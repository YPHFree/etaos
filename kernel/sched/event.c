/*
 *  ETA/OS - Event management
 *  Copyright (C) 2015   Michel Megens
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

/**
 * @addtogroup evm
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/preempt.h>
#include <etaos/cpu.h>
#include <etaos/event.h>
#include <etaos/irq.h>
#include <etaos/bitops.h>
#include <etaos/time.h>
#include <etaos/spinlock.h>

/**
 * @brief Signal the queue head of a thread_queue.
 * @param rq Run queue to place a woken up thread on.
 * @param qp Thread queue which has to be signaled.
 */
static void __raw_event_notify(struct rq *rq, struct thread_queue *qp)
{
	struct thread *tp;
	unsigned long flags;

	irq_save_and_disable(&flags);
	tp = qp->qhead;

	rq_remove_wake_thread(rq, tp);
	queue_remove_thread(qp, tp);
	if(!qp->qhead)
		qp->qhead = SIGNALED;

	if(tp->timer && tp->timer != SIGNALED) {
		tm_stop_timer(tp->timer);
		tp->timer = NULL;
	}

	if(rq->current != tp) {
		rq_add_thread(rq, tp);
		if(prio(tp) <= prio(rq->current))
			set_bit(THREAD_NEED_RESCHED_FLAG, &rq->current->flags);
	} else {
		set_bit(THREAD_RUNNING_FLAG, &tp->flags);
		tp->on_rq = true;
		tp->rq = rq;
	}
	irq_restore(&flags);
}

static void event_tmo(struct timer *timer, void *arg)
{
	struct thread *walker, *curr;
	struct thread_queue *qp;
	struct thread *volatile*tpp = arg;

	qp = container_of((struct thread**)tpp, struct thread_queue, qhead);
	walker = *tpp;

	if(walker != SIGNALED) {
		while(walker) {
			if(walker->timer == timer) {
				raw_rq_remove_wake_thread(sched_get_cpu_rq(), 
							  walker);
				queue_remove_thread(qp, walker);
				if(!qp->qhead)
					qp->qhead = SIGNALED;

				walker->timer = SIGNALED;
				rq_add_thread_no_lock(walker);
				break;
			}
			walker = qp->sched_class->thread_after(walker);
		}

		curr = current_thread();
		if(prio(walker) <= prio(curr))
			set_bit(THREAD_NEED_RESCHED_FLAG, &curr->flags);
	}
}

/**
 * @brief Signal an event queue.
 * @param qp thread_queue to signal.
 * @warning NEVER call this from an ISR.
 */
void event_notify(struct thread_queue *qp)
{
	struct rq *rq;
	struct thread *tp;
	unsigned long flags;

	cpu_get_state(&flags);
	if(test_bit(CPU_IRQ_EXEC_FLAG, &flags)) {
		event_notify_irq(qp);
		return;
	}

	tp = qp->qhead;

	preempt_disable();
	if(!tp) {
		raw_spin_lock_irq(&qp->lock);
		qp->qhead = SIGNALED;
		raw_spin_unlock_irq(&qp->lock);
	} else if(tp != SIGNALED) {
		rq = sched_get_cpu_rq();
		__raw_event_notify(rq, qp);
	}
	preempt_enable_no_resched();

	yield();
}

/**
 * @brief Wait for the next event on an event queue.
 * @param qp Queue to wait on.
 * @param ms Maximum time to wait in miliseconds.
 * @note Set ms to 0 to wait infinitly.
 * @see EVM_WAIT_INFINITE
 */
int event_wait(struct thread_queue *qp, unsigned ms)
{
	int rv;

	preempt_disable();
	raw_spin_unlock_irq(&qp->lock);
	if(qp->qhead == SIGNALED)
		qp->qhead = NULL;
	raw_spin_unlock_irq(&qp->lock);

	preempt_enable_no_resched();
	rv = raw_event_wait(qp, ms);

	return rv;
}

/**
 * @brief Wait for an event in a specified queue.
 * @param qp Queue to wait in.
 * @param ms Maximum time to wait in miliseconds.
 *
 * Give up the CPU untill an event is posted to this queue, or a timeout based
 * on \p ms occurs.
 */
int raw_event_wait(struct thread_queue *qp, unsigned ms)
{
	struct thread *tp;
	unsigned long flags;
	struct clocksource *cs;

	preempt_disable();
	raw_spin_lock_irqsave(&qp->lock, flags);
	if(qp->qhead == SIGNALED) {
		qp->qhead = NULL;
		raw_spin_unlock_irqrestore(&qp->lock, flags);
		preempt_enable_no_resched();
		yield();
		return -EOK;
	} else {
		raw_spin_unlock_irqrestore(&qp->lock, flags);
	}

	tp = current_thread();
	cs = tp->rq->source;

	if(ms)
		tp->timer = tm_create_timer(cs, ms, &event_tmo,
				(void*)&qp->qhead, TIMER_ONESHOT_MASK);
	else
		tp->timer = NULL;

	thread_add_to_wake_q(tp);
	queue_add_thread(qp, tp);
	preempt_enable_no_resched();
	schedule();

	tp = current_thread();
	if(tp->timer == SIGNALED) {
		tp->timer = NULL;
		return -1;
	}

	return 0;
}

/**
 * @brief Signal an event from an IRQ.
 * @param qp Queue to signel.
 */
void event_notify_irq(struct thread_queue *qp)
{
	struct thread *tp;

	tp = qp->qhead;
	if(!tp)
		qp->qhead = SIGNALED;
	else if(tp != SIGNALED)
		tp->ec++;
}

/* @} */

