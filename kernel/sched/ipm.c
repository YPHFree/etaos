/*
 *  ETA/OS - Inter-Process Messages
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
#include <etaos/thread.h>
#include <etaos/types.h>
#include <etaos/ipm.h>
#include <etaos/mem.h>
#include <etaos/evm.h>

void ipm_queue_init(struct ipm_queue *iq, size_t len)
{
	void *ptr;

	thread_queue_init(&iq->qp);
	ptr = kzalloc(sizeof(*iq->msgs) * len);
	if(!ptr)
		return;

	iq->msgs = ptr;
	iq->num = len;
	iq->wr_idx = 0;
	iq->rd_idx = 0;
}

void ipm_post_msg(struct ipm_queue *iq, void *buff, size_t len)
{
	unsigned long flags;
	struct ipm *msg;

	spin_lock_irqsave(&iq->lock, flags);
	msg = &iq->msgs[iq->wr_idx];
	iq->wr_idx += 1;
	spin_unlock_irqrestore(&iq->lock, flags);

	msg->data = buff;
	msg->len = len;

	evm_signal_event_queue(&iq->qp);
}

