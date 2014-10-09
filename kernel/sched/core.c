/*
 *  ETA/OS - Scheduling core
 *  Copyright (C) 2014   Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/irq.h>
#include <etaos/bitops.h>
#include <etaos/time.h>
#include <etaos/spinlock.h>

#include <asm/io.h>

void thread_wake_up_from_irq(struct thread *thread)
{
}

static inline struct thread *sched_next_runnable(struct rq *rq)
{
	struct sched_class *class;

	class = rq->sched_class; /* class is valid, a thread 
			       CANNOT be running wihout */
	if(class->next_runnable)
		return class->next_runnable(rq);

	return NULL;
}

static int rq_list_remove(struct thread *tp, struct thread *volatile*tpp)
{
	struct thread *carriage;

	carriage = *tpp;
	if(carriage == SIGNALED)
		return -EINVAL;

	while(carriage) {
		if(carriage == tp) {
			*tpp = tp->rq_next;
			tp->rq_next = NULL;
			tp->queue = NULL;
			break;
		}

		tpp = &carriage->rq_next;
		continue;
	}

	return -EOK;
}

void raw_rq_remove_wake_thread(struct rq *rq, struct thread *tp)
{
	unsigned long flags;

	irq_save_and_disable(&flags);
	rq_list_remove(tp, &rq->wake_queue);
	irq_restore(&flags);
}

void raw_rq_remove_kill_thread(struct rq *rq, struct thread *tp)
{
	unsigned long flags;

	irq_save_and_disable(&flags);
	rq_list_remove(tp, &rq->kill_queue);
	irq_restore(&flags);
}

void rq_remove_wake_thread(struct rq *rq, struct thread *tp)
{
	spin_lock(&rq->lock);
	raw_rq_remove_wake_thread(rq, tp);
	spin_unlock(&rq->lock);
}

void rq_remove_kill_thread(struct rq *rq, struct thread *tp)
{
	spin_lock(&rq->lock);
	raw_rq_remove_kill_thread(rq, tp);
	spin_unlock(&rq->lock);
}

static inline void rq_list_add(struct thread *new, struct thread *head)
{
	new->rq_next = head;
}

static void rq_add_wake_thread(struct rq *rq, struct thread *new)
{
	struct sched_class *class;

	class = rq->sched_class;

	if(new->on_rq)
		class->rm_thread(new->rq, new);

	rq_list_add(new, rq->wake_queue);
	rq->wake_queue = new;
}

static void rq_add_kill_thread(struct rq *rq, struct thread *new)
{
	struct sched_class *class;

	class = rq->sched_class;

	if(new->on_rq)
		class->rm_thread(new->rq, new);

	rq_list_add(new, rq->kill_queue);
	rq->kill_queue = new;
}

void raw_thread_add_to_wake_q(struct thread *tp)
{
	struct rq *rq;

	if(!tp->on_rq)
		return;

	rq = tp->rq;
	rq_add_wake_thread(rq, tp);
}

void raw_thread_add_to_kill_q(struct thread *tp)
{
	struct rq *rq;

	if(!tp->on_rq)
		return;

	rq = tp->rq;
	rq_add_kill_thread(rq, tp);
}

void thread_add_to_wake_q(struct thread *tp)
{
	struct rq *rq;

	if(!tp->on_rq)
		return;

	rq = tp->rq;
	spin_lock(&rq->lock);
	raw_thread_add_to_wake_q(tp);
	spin_unlock(&rq->lock);
}

void thread_add_to_kill_q(struct thread *tp)
{
	struct rq *rq;

	if(!tp->on_rq)
		return;

	rq = tp->rq;
	spin_lock(&rq->lock);
	raw_thread_add_to_kill_q(tp);
	spin_unlock(&rq->lock);
}

int rq_add_thread(struct rq *rq, struct thread *tp)
{
	int err;
	struct prev_rq;
	struct sched_class *class = rq->sched_class;

	err = 0;
	if(tp->on_rq)
		err = rq_remove_thread(tp);

	spin_lock(&rq->lock);
	class->add_thread(rq, tp);
	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	spin_unlock(&rq->lock);
	tp->on_rq = true;

	return err;
}

int rq_remove_thread(struct thread *tp)
{
	int err;
	struct sched_class *class;
	struct rq *rq;

	rq = tp->rq;
	if(!tp->on_rq) {
		err = -EINVAL;
	} else {
		class = rq->sched_class;
		spin_lock(&rq->lock);
		clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
		err = class->rm_thread(rq, tp);
		spin_unlock(&rq->lock);
		tp->on_rq = false;
	}

	return err;
}

static void rq_post_schedule(struct rq *rq)
{
}

static inline struct thread *sched_get_next_runnable(struct rq *rq)
{
	struct sched_class *class = rq->sched_class;

	if(class)
		return class->next_runnable(rq);
	else
		return NULL;
}

#define current(_rq) ((_rq)->current)
static void rq_schedule(void)
{
	struct rq *rq;
	int64_t diff;
	struct thread *tp;
	unsigned long flags;
#ifdef CONFIG_EVENT_MUTEX
	struct thread *carriage, *volatile*tpp;
	unsigned char events;
#endif

	rq = sched_get_cpu_rq();

resched:
	preemt_disable(current(rq));
	spin_lock_irqsave(&rq->lock, flags);
#ifdef CONFIG_EVENT_MUTEX
	carriage = rq->wake_queue;
	while(carriage) {
		events = carriage->ec;

		if(events) {
			tpp = carriage->queue;
			carriage->ec--;
			tp = *tpp;
			if(tp != SIGNALED)
				thread_wake_up_from_irq(tp);
		}
		carriage = carriage->rq_next;
	}
#endif

#ifdef CONFIG_TIMER
	diff = tm_update_source(rq->source);
	if(diff)
		tm_process_clock(rq->source, diff);
#endif

	tp = sched_get_next_runnable(rq);
	if(test_and_clear_bit(THREAD_NEED_RESCHED_FLAG,
				&current(rq)->flags) && tp != current(rq)) {
		rq->current = tp;
		rq->switch_count++;
		cpu_reschedule(rq, current(rq), tp);
		spin_unlock_irqrestore(&rq->lock, flags);
		rq_post_schedule(rq);
		rq = sched_get_cpu_rq();
	} else {
		spin_unlock_irqrestore(&rq->lock, flags);
	}

	preemt_enable_no_resched(current(rq));
	if(test_bit(THREAD_NEED_RESCHED_FLAG, &current(rq)->flags))
		goto resched;

	return;
}

void schedule(void)
{
	rq_schedule();
}

