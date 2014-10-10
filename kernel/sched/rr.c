/*
 *  ETA/OS - Round robin sched class
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
#include <etaos/sched.h>
#include <etaos/thread.h>

static void rr_queue_insert(struct thread *volatile*tpp, struct thread *tp)
{
	struct thread *thread;

	tp->ec = 0;
	tp->queue = tpp;
	
	thread = *tpp;

	if(thread == SIGNALED) {
		thread = NULL;
		tp->ec++;
	} else if(thread) {
		while(thread && thread->prio < tp->prio) {
			tpp = &thread->se.next;
			thread = thread->se.next;
		}
	}
	tp->se.next = thread;
	*tpp = tp;

	if(tp->se.next && tp->se.next->ec) {
		tp->ec += tp->se.next->ec;
		tp->se.next->ec = 0;
	}
}

static int rr_queue_remove(struct thread *volatile*tpp, struct thread *tp)
{
	struct thread *thread;
	int err = -EINVAL;

	thread = *tpp;

	if(thread == SIGNALED)
		return err;

	while(thread) {
		if(thread == tp) {
			err = 0;
			*tpp = tp->se.next;
			if(tp->ec) {
				if(tp->se.next)
					tp->se.next->ec = tp->ec;
				tp->ec = 0;
			}

			tp->se.next = NULL;
			tp->queue = NULL;
			break;
		}

		tpp = &thread->se.next;
		thread = thread->se.next;
	}

	return err;
}

static void rr_thread_queue_add(struct thread_queue *qp, struct thread *tp)
{
	rr_queue_insert(&qp->qhead, tp);
}

static void rr_thread_queue_remove(struct thread_queue *qp, struct thread *tp)
{
	rr_queue_remove(&qp->qhead, tp);
}

static void rr_add_thread(struct rq *rq, struct thread *tp)
{
	rr_queue_insert(&rq->rq.run_queue, tp);
}

static int rr_rm_thread(struct rq *rq, struct thread *tp)
{
	return rr_queue_remove(&rq->rq.run_queue, tp);
}

static struct thread *rr_next_runnable(struct rq *rq)
{
	struct thread *runnable;

	for(runnable = rq->rq.run_queue; runnable; 
			runnable = runnable->se.next) {
		if(!test_bit(THREAD_RUNNING_FLAG, &runnable->flags))
			continue;
		else
			break;
	}
	
	return runnable;
}

struct sched_class sys_sched_class = {
	.rm_thread = &rr_rm_thread,
	.add_thread = &rr_add_thread,
	.next_runnable = &rr_next_runnable,
	.queue_add = &rr_thread_queue_add,
	.queue_rm = &rr_thread_queue_remove,
};

