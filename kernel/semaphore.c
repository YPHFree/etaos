/*
 *  ETA/OS - Semaphore
 *  Copyright (C) 2014, 2015   Michel Megens <dev@bietje.net>
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

/**
 * @addtogroup sem
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/event.h>
#include <etaos/types.h>
#include <etaos/semaphore.h>

/**
 * @brief Initialise a a semaphore with a value.
 * @param sem Semaphore to initialise.
 * @param value Initial semaphore value.
 * @return Error code.
 * @retval -EINVAL on error.
 * @retval -EOK on success.
 */
int sem_init(sem_t *sem, short value)
{
	if(!sem)
		return -EINVAL;

	sem->qp.qhead = NULL;
	sem->value = value;
	return -EOK;
}

/**
 * @brief Lock a semaphore.
 * @param sem Semaphore to lock.
 *
 * If the value of the semaphore is 0 or less, the calling thread will not
 * return from this function untill the semaphore becomes available.
 */
void sem_wait(sem_t *sem)
{
	sem->value--;
	if(sem->value < 0)
		raw_event_wait(&sem->qp, EVM_WAIT_INFINITE);
}

/**
 * @brief Signal a locked semaphore.
 * @param sem Semaphore to unlock.
 *
 * Unlocks a held semaphore.
 */
void sem_signal(sem_t *sem)
{
	sem->value++;
	if(sem->value <= 0)
		event_notify(&sem->qp);
}

/**
 * @brief Try to lock a semaphore without waiting.
 * @param sem Semaphore to try.
 * @return Error code.
 * @retval -1 if semaphore couldn't be acquired.
 * @retval 0 if The semaphore succesfully locked.
 */
int sem_try_wait(sem_t *sem)
{
	if(sem->value <= 0)
		return -1;
	else
		sem_wait(sem);

	return 0;
}

/* @} */

