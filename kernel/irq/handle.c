/*
 *  ETA/OS - IRQ handling
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
#include <etaos/list.h>
#include <etaos/irq.h>
#include <etaos/error.h>
#include <etaos/bitops.h>

static void irq_handle_hard_irq(struct irq_data *data)
{
}

static void irq_handle_slow_irq(struct irq_data *data)
{
}

void irq_handle(int irq)
{
	struct irq_data *data;

	data = irq_to_data(irq);
	if(!data || !test_bit(IRQ_ENABLE_FLAG, &data->flags))
		return;

	if(test_bit(IRQ_SLOW_SOFT_FLAG, &data->flags))
		irq_handle_slow_irq(data);
	else
		irq_handle_hard_irq(data);
}
