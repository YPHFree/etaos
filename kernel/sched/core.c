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
#include <etaos/init.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/preempt.h>
#include <etaos/irq.h>
#include <etaos/bitops.h>
#include <etaos/time.h>
#include <etaos/spinlock.h>

#include <asm/io.h>
#include <asm/sched.h>

static void raw_rq_add_thread(struct rq *rq, struct thread *tp);
static int raw_rq_remove_thread(struct rq *rq, struct thread *tp);

void thread_wake_up_from_irq(struct thread *thread)
{
}

#ifdef CONFIG_THREAD_QUEUE
static void raw_queue_add_thread(struct thread_queue *qp,
					struct thread *tp)
{
	struct sched_class *cp;
	unsigned long flags;

	if(tp->on_rq && test_bit(THREAD_RUNNING_FLAG, &tp->flags)) {
		raw_spin_lock_irqsave(&tp->rq->lock, flags);
		raw_rq_remove_thread_noresched(tp->rq, tp);
		spin_unlock_irqrestore(&tp->rq->lock, flags);
	}


	cp = qp->sched_class;
	if(cp)
		cp->queue_add(qp, tp);
}

static void raw_queue_remove_thread(struct thread_queue *qp,
						struct thread *tp)
{
	struct sched_class *cp;

	cp = qp->sched_class;
	if(cp)
		cp->queue_rm(qp, tp);
}

void queue_add_thread(struct thread_queue *qp, struct thread *tp)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&qp->lock, flags);
	raw_queue_add_thread(qp, tp);
	spin_unlock_irqrestore(&qp->lock, flags);
	schedule();
}

void queue_remove_thread(struct thread_queue *qp, struct thread *tp)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&qp->lock, flags);
	raw_queue_remove_thread(qp, tp);
	spin_unlock_irqrestore(&qp->lock, flags);
	schedule();
}
#endif

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
	raw_spin_lock(&rq->lock);
	raw_rq_remove_wake_thread(rq, tp);
	raw_spin_unlock(&rq->lock);
}

void rq_remove_kill_thread(struct rq *rq, struct thread *tp)
{
	raw_spin_lock(&rq->lock);
	raw_rq_remove_kill_thread(rq, tp);
	raw_spin_unlock(&rq->lock);
}

static inline void rq_list_add(struct thread *new, struct thread *head)
{
	new->rq_next = head;
}

static void rq_add_wake_thread(struct rq *rq, struct thread *new)
{
	new->rq = rq;
	rq_list_add(new, rq->wake_queue);
	rq->wake_queue = new;
}

static void rq_add_kill_thread(struct rq *rq, struct thread *new)
{
	new->rq = rq;
	rq_list_add(new, rq->kill_queue);
	rq->kill_queue = new;
}

void raw_thread_add_to_wake_q(struct thread *tp)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	rq_add_wake_thread(rq, tp);
}

void raw_thread_add_to_kill_q(struct thread *tp)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	rq_add_kill_thread(rq, tp);
}

void thread_add_to_wake_q(struct thread *tp)
{
	struct rq *rq;
	unsigned long flags;

	rq = sched_get_cpu_rq();
	raw_spin_lock_irqsave(&rq->lock, flags);
	raw_thread_add_to_wake_q(tp);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}

void thread_add_to_kill_q(struct thread *tp)
{
	struct rq *rq;
	unsigned long flags;

	rq = sched_get_cpu_rq();
	raw_spin_lock_irqsave(&rq->lock, flags);
	raw_thread_add_to_kill_q(tp);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}

static void raw_rq_add_thread(struct rq *rq, struct thread *tp)
{
	struct prev_rq;
	struct sched_class *class = rq->sched_class;

	class->add_thread(rq, tp);
	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	tp->on_rq = true;
	tp->rq = rq;

	return;
}

int rq_add_thread(struct rq *rq, struct thread *tp)
{
	int err;
	struct prev_rq;
	unsigned long flags;
	struct sched_class *class = rq->sched_class;

	err = 0;
	if(tp->on_rq && test_bit(THREAD_RUNNING_FLAG, &tp->flags))
		err = rq_remove_thread(tp);

	raw_spin_lock_irqsave(&rq->lock, flags);
	class->add_thread(rq, tp);
	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
	tp->on_rq = true;
	tp->rq = rq;

	return err;
}

int raw_rq_remove_thread_noresched(struct rq *rq, struct thread *tp)
{
	struct sched_class *class;
	int err;

	if(!tp->on_rq) {
		err = -EINVAL;
	} else if(rq->current == tp) {
		/*
		 * The current thread is to be
		 * removed from the RQ. This requires
		 * it to be stopped first.
		 */
		clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
		set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
		err = 1;
	} else {
		class = rq->sched_class;
		clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
		err = class->rm_thread(rq, tp);
		tp->on_rq = false;
	}

	return err;
}

static int raw_rq_remove_thread(struct rq *rq, struct thread *tp)
{
	int err;

	err = raw_rq_remove_thread_noresched(rq, tp);
	
	if(err > 0) {
		schedule();
		err = 0;
	}

	return err;
}

int rq_remove_thread(struct thread *tp)
{
	int err;
	struct rq *rq;
	unsigned long flags;

	rq = tp->rq;
	if(rq->current != tp) {
		raw_spin_lock_irqsave(&rq->lock, flags);
		err = raw_rq_remove_thread(rq, tp);
		raw_spin_unlock_irqrestore(&rq->lock, flags);
	} else {
		err = raw_rq_remove_thread(rq, tp);
	}

	return err;
}

static void rq_update(struct rq *rq)
{
	struct sched_class *class;

	class = rq->sched_class;
	if(class->post_schedule)
		class->post_schedule(rq);
}

static void sched_sleep_timeout(struct timer *timer, void *arg)
{
	struct thread *tp;
	struct rq *rq;

	tp = arg;
	rq = sched_get_cpu_rq();

	clear_bit(THREAD_SLEEPING_FLAG, &tp->flags);
	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	raw_rq_add_thread(rq, tp);
}

void sched_setup_sleep_thread(struct thread *tp, unsigned ms)
{
	struct rq *rq;

	rq = tp->rq;
	set_bit(THREAD_SLEEPING_FLAG, &tp->flags);
	clear_bit(THREAD_RUNNING_FLAG, &tp->flags);

	tp->timer = tm_create_timer(rq->source, ms, &sched_sleep_timeout,
			tp, TIMER_ONESHOT_MASK);
}

static struct thread *sched_get_next_runnable(struct rq *rq,
						     struct thread *current)
{
	struct thread *next;
	struct sched_class *class = rq->sched_class;

	if(class) {
		next = class->next_runnable(rq);
		if(!next)
			next = rq->current;

		return next;
	} else {
		return rq->current;
	}
}

#ifdef CONFIG_DYN_PRIO
#define dyn_prio_reset(__t) (__t)->dprio = 0;
#else
#define dyn_prio_reset(__t)
#endif
static void rq_switch_context(struct rq *rq, struct thread *prev,
						struct thread *new)
{
	struct sched_class *class = rq->sched_class;

	if(prev) {
		if(test_bit(THREAD_RUNNING_FLAG, &prev->flags)) {
			prev->rq = rq;
			prev->on_rq = true;
			class->add_thread(rq, prev);
		} else {
			prev->rq = NULL;
			prev->on_rq = false;
		}
	}

	if(prev != new) {
		class->rm_thread(rq, new); /* remove the new 
					      thread from the rq */
		raw_spin_unlock_irq(&rq->lock);
		cpu_reschedule(rq, prev, new);
		dyn_prio_reset(new);
	} else {
		raw_spin_unlock_irq(&rq->lock);
	}

}

#ifdef CONFIG_EVENT_MUTEX
static void rq_signal_event_queue(struct rq *rq, struct thread *tp)
{
	struct thread_queue *qp;
	unsigned long flags;

	irq_save_and_disable(&flags);
	qp = thread_to_queue(tp);
	tp = *tp->queue;

	raw_rq_remove_wake_thread(rq, tp);
	raw_queue_remove_thread(qp, tp);

	if(rq->current != tp) {
		raw_rq_add_thread(rq, tp);
		if(prio(tp) <= prio(rq->current))
			set_bit(THREAD_NEED_RESCHED_FLAG, &rq->current->flags);
	} else {
		set_bit(THREAD_RUNNING_FLAG, &tp->flags);
		tp->on_rq = true;
		tp->rq = rq;
	}
	irq_restore(&flags);
}
#endif

#define current(_rq) ((_rq)->current)
static bool rq_schedule(void)
{
	struct rq *rq;
	int64_t diff;
	bool did_switch;
	struct thread *tp, *prev;
#ifdef CONFIG_EVENT_MUTEX
	struct thread *carriage, *volatile*tpp;
	unsigned char events;
#endif

	rq = sched_get_cpu_rq();
	prev = current(rq);
	did_switch = false;

resched:
	preempt_disable();
	raw_spin_lock_irq(&rq->lock);

#ifdef CONFIG_EVENT_MUTEX
	carriage = rq->wake_queue;
	while(carriage) {
		events = carriage->ec;

		if(events) {
			tpp = carriage->queue;
			carriage->ec--;
			tp = *tpp;
			if(tp != SIGNALED)
				rq_signal_event_queue(rq, tp);
		}
		carriage = carriage->rq_next;
	}
#endif

#ifdef CONFIG_TIMER
	diff = tm_update_source(rq->source);
	if(diff)
		tm_process_clock(rq->source, diff);
#ifdef CONFIG_PREEMPT
	if(diff < prev->slice) {
		prev->slice -= diff;
	} else {
		prev->slice = CONFIG_TIME_SLICE;
		set_bit(THREAD_NEED_RESCHED_FLAG, &prev->flags);
	}
#endif
#endif

	tp = sched_get_next_runnable(rq, rq->current);

	/* Only have to reschedule if the THREAD_NEED_RESCHED_FLAG
	   is set on the current thread, and iff the next runnable
	   isn't the same thread as the one currently running. */
	if(test_and_clear_bit(THREAD_NEED_RESCHED_FLAG,
			&current(rq)->flags) && tp != current(rq)) {
		rq->current = tp;
		rq->switch_count++;
		rq_switch_context(rq, prev, tp);
		did_switch = true;
		rq = sched_get_cpu_rq();
		prev = current(rq);
		rq_update(rq);
	} else {
		raw_spin_unlock_irq(&rq->lock);
	}

	preempt_enable_no_resched();
	if(test_bit(THREAD_NEED_RESCHED_FLAG, &prev->flags))
		goto resched;

	return did_switch;
}

void schedule(void)
{
#ifdef CONFIG_DYN_PRIO
	bool need_prio_update;
	struct rq *rq;
	
	need_prio_update = rq_schedule();
	rq = sched_get_cpu_rq();
	if(need_prio_update && rq->sched_class->dyn_prio_update) {
		raw_spin_lock_irq(&rq->lock);
		rq->sched_class->dyn_prio_update(rq);
		raw_spin_unlock_irq(&rq->lock);
	}
#else
	rq_schedule();
#endif
}

static struct thread idle_thread, main_thread;
#define CONFIG_IDLE_STACK_SIZE CONFIG_STACK_SIZE
static uint8_t idle_stack[CONFIG_IDLE_STACK_SIZE];

THREAD(idle_thread_func, arg)
{
	struct thread *tp = arg;

	thread_initialise(&main_thread, "main", &main_thread_func, &main_thread,
			CONFIG_STACK_SIZE, main_stack_ptr, 120);
	while(true) {
		set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
		schedule();
	}
}

void sched_init(void)
{
	struct rq *rq;

	sched_init_idle(&idle_thread, "idle", &idle_thread_func,
			&idle_thread, CONFIG_IDLE_STACK_SIZE, idle_stack);
	rq = sched_get_cpu_rq();
	idle_thread.rq = rq;
	rq->current = &idle_thread;

	cpu_reschedule(rq, NULL, &idle_thread);
}

#ifdef CONFIG_PREEMPT
int *preempt_counter_ptr(void)
{
	struct thread *tp = current_thread();
	return &tp->preemt_cnt;
}

void __preempt_add(int num)
{
	volatile int *preemt_ptr = preempt_counter_ptr();

	*preemt_ptr += num;
}

void __preempt_sub(int num)
{
	volatile int *preemt_ptr = preempt_counter_ptr();

	*preemt_ptr -= num;
}
#endif

bool should_resched(void)
{
	struct thread *tp = current_thread();

	if(test_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags))
		return true;
	else
		return false;
}

#ifdef CONFIG_DYN_PRIO
unsigned char prio(struct thread *tp)
{
	unsigned char retval;

	retval = tp->prio;
	if(tp->dprio <= retval)
		retval -= tp->dprio;
	else
		retval = 0;

	return retval;
}
#else
unsigned char prio(struct thread *tp)
{
	if(tp)
		return tp->prio;
	else
		return 0;
}
#endif

