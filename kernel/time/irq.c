/*
 *  ETA/OS - Time core
 *  Copyright (C) 2014, 2015   Michel Megens
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
 * @file kernel/time/irq.c
 * @addtogroup tm
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/atomic.h>
#include <etaos/time.h>
#include <etaos/sched.h>
#include <etaos/preempt.h>
#include <etaos/irq.h>
#include <etaos/tick.h>
#include <etaos/stdio.h>
#include <etaos/hrtimer.h>

struct clocksource *sys_clk;

/**
 * @brief System tick IRQ handler.
 * @param irq IRQ data.
 * @param data Private data.
 * @return IRQ handle status.
 * @retval IRQ_HANDLED
 */
static irqreturn_t timer_irq_handle(struct irq_data *irq, void *data)
{
	struct clocksource *cs = (struct clocksource*)data;

	time_inc(); /* Handle system time */
	timer_source_inc(cs); /* Increase the system / sched clock */
	sched_clock_tick((1.0f / cs->freq) * 1000);

	return IRQ_HANDLED;
}

/**
 * @brief Setup the system tick.
 * @param irq IRQ bound to the system tick.
 * @param src Clock source handling the system tick.
 */
void systick_setup(int irq, struct clocksource *src)
{
	irq_request(irq, &timer_irq_handle, IRQ_RISING_MASK, src);
}

#ifdef CONFIG_HRTIMER
/**
 * @brief High resolution timer interrupt.
 * @param data IRQ data.
 * @param arg IRQ argument (clocksource).
 * @return IRQ handle status.
 * @retval IRQ_HANDLED
 */
irqreturn_t hrtimer_tick(struct irq_data *data, void *arg)
{
	struct clocksource *src;

	src = arg;
	
	timer_source_inc(src);
	hrtimer_handle(src);
	return IRQ_HANDLED;
}
#endif

/** @} */

