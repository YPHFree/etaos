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
#include <etaos/spinlock.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>

static void raw_thread_init(struct thread *tp, char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack, unsigned char prio)
{
	tp->name = name;
	tp->param = arg;
	tp->preemt_cnt = 0;
	tp->prio = prio;
	tp->flags = 0;
#ifdef CONFIG_EVENT_MUTEX
	tp->ec = 0;
#endif

	tp->rq_next = NULL;
	tp->queue = NULL;
	tp->timer = NULL;
	tp->se.next = NULL;

	sched_create_stack_frame(tp, stack, stack_size, handle);

	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
}

void sched_init_idle(struct thread *tp, char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack)
{
	raw_thread_init(tp, name, handle, arg, stack_size, stack, 255);
}

struct thread *thread_create(char *name, thread_handle_t handle, void *arg,
			size_t stack_size, void *stack, unsigned char prio)
{
	struct thread *tp;

	tp = kmalloc(sizeof(*tp));
	if(!tp)
		return NULL;

	if(thread_initialise(tp, name, handle, arg, stack_size, stack, prio))
		return NULL;
	return tp;
}

int thread_initialise(struct thread *tp, char *name, thread_handle_t handle, 
		void *arg, size_t stack_size, void *stack, unsigned char prio)
{
	struct rq *rq;

	if(!tp)
		return -EINVAL;

	raw_thread_init(tp, name, handle, arg, stack_size, stack, prio);
	rq = sched_select_rq();
	
	rq_add_thread(rq, tp);
	return -EOK;
}

void yield(void)
{
	struct rq *rq;
	struct thread *tp;

	rq = sched_get_cpu_rq();
	tp = rq->sched_class->next_runnable(rq);
	if(!rq->current->preemt_cnt && tp) {
#ifdef CONFIG_DYN_PRIO
		if(sched_dyn_prio(tp) <= sched_dyn_prio(rq->current)) {
#else
		if(tp->prio <= rq->current->prio) {
#endif
			set_bit(THREAD_NEED_RESCHED_FLAG, &rq->current->flags);
		}
		
		if(test_bit(THREAD_NEED_RESCHED_FLAG, &rq->current->flags))
			schedule();
	}
}

