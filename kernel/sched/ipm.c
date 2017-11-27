/*
 *  ETA/OS - Inter-Process Messages
 *  Copyright (C) 2014, 2015, 2016, 2017  Michel Megens <dev@bietje.net>
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
 * @addtogroup ipm
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/types.h>
#include <etaos/ipm.h>
#include <etaos/mem.h>
#include <etaos/event.h>

static inline bool ipm_queue_is_full(struct ipm_queue *iq)
{
	if(iq->wr_idx >= iq->num)
		return true;
	else
		return false;
}

/**
 * @brief Initialise a new IPM queue.
 * @param iq Queue to initialise.
 * @param len Amount of messages to allocate.
 */
void ipm_queue_init(struct ipm_queue *iq, size_t len)
{
	void *ptr;

	thread_queue_init(&iq->qp);
	spinlock_init(&iq->lock);
	iq->qp.qhead = NULL;
	ptr = kzalloc(sizeof(*iq->msgs) * len);
	if(!ptr)
		return;

	iq->msgs = ptr;
	iq->num = len;
	iq->wr_idx = 0;
	iq->rd_idx = 0;
}

/**
 * @brief Send a message to queue.
 * @param iq Queue to post the message to.
 * @param buff Message to send.
 * @param len Length of \p buff.
 * @return An error code.
 */
int ipm_post_msg(struct ipm_queue *iq, const void *buff, size_t len)
{
	unsigned long flags;
	struct ipm *msg;

	if(ipm_queue_is_full(iq))
		return -EINVAL;

	spin_lock_irqsave(&iq->lock, flags);
	msg = &iq->msgs[iq->wr_idx];
	iq->wr_idx += 1;
	spin_unlock_irqrestore(&iq->lock, flags);

	msg->data = buff;
	msg->len = len;

	event_notify(&iq->qp);
	return -EOK;
}

/**
 * @brief Get a message from a queue.
 * @param iq Queue to get the message from.
 * @param msg Memory to store the message into.
 * @return An error code.
 */
int ipm_get_msg(struct ipm_queue *iq, struct ipm *msg)
{
	unsigned long flags;

	/* check if there are messages available */
	spin_lock_irqsave(&iq->lock, flags);
	if(iq->rd_idx >= iq->wr_idx) {
		spin_unlock_irqrestore(&iq->lock, flags);
		raw_event_wait(&iq->qp, EVM_WAIT_INFINITE);

		spin_lock_irqsave(&iq->lock, flags);
		if(iq->wr_idx == iq->rd_idx) {
			spin_unlock_irqrestore(&iq->lock, flags);
			return -EINVAL;
		}
	}

	*msg = iq->msgs[iq->rd_idx];
	iq->rd_idx += 1;

	if(iq->rd_idx != iq->wr_idx) {
		spin_unlock_irqrestore(&iq->lock, flags);
		event_notify(&iq->qp);
		return -EOK;
	}

	spin_unlock_irqrestore(&iq->lock, flags);
	return -EOK;
}

/**
 * @brief Reset a message queue.
 * @param iq Queue which has to be reset.
 * @return True or false based on whether there are still threads waiting.
 * @retval false if there are still threads waiting.
 * @retval true if the queue reset was succesfull.
 */
bool ipm_reset_queue(struct ipm_queue *iq)
{
	unsigned long flags;

	spin_lock_irqsave(&iq->lock, flags);
	if(iq->qp.qhead != SIGNALED && iq->qp.qhead != NULL) {
		spin_unlock_irqrestore(&iq->lock, flags);
		return false;
	}

	iq->qp.qhead = NULL;
	iq->wr_idx = 0;
	iq->rd_idx = 0;
	spin_unlock_irqrestore(&iq->lock, flags);

	return true;
}
/* @} */
