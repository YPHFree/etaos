/*
 *  ETA/OS - Mutexes
 *  Copyright (C) 2014, 2015, 2016, 2017   Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/types.h>
#include <etaos/thread.h>
#include <etaos/event.h>
#include <etaos/mutex.h>
#include <etaos/panic.h>

#include <asm/pgm.h>

void mutex_wait(mutex_t *mutex)
{
	event_wait(&mutex->qp, EVENT_WAIT_INFINITE);
}

int mutex_wait_tmo(mutex_t *mutex, unsigned int tmo)
{
	return event_wait(&mutex->qp, tmo);
}

void mutex_lock(mutex_t *mutex)
{
	struct thread *tp = current_thread();

	if(mutex->owner != tp) {
		while(mutex->count != 0)
			raw_event_wait(&mutex->qp, EVENT_WAIT_INFINITE);
	} 

	preempt_disable();
	mutex->count++;
	mutex->owner = tp;
}

void mutex_unlock(mutex_t *mutex)
{
	struct thread *tp = current_thread();

	if(mutex->owner != tp)
		panic_P(PSTR("Invalid mutex unlock\n"));

	if(--mutex->count == 0) {
		mutex->owner = NULL;
		event_notify(&mutex->qp);
	}

	preempt_enable();
}

void mutex_unlock_irq(mutex_t *mutex)
{
	event_notify_irq(&mutex->qp);
}

