/*
 *  ETA/OS - Timer header
 *  Copyright (C) 2014, 2015   Michel Megens
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
 * @file etaos/time.h Time header.
 * @addtogroup tm
 * @{
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>
#include <etaos/spinlock.h>
#include <etaos/atomic.h>

struct timer;
/**
 * @struct clocksource
 * @brief The clocksource describes the source of a hardware time.
 */
struct clocksource {
	const char *name; //!< Timer name.

	/**
	 * @brief Enable the timer.
	 * @param cs Clocksource to enable.
	 */
	int (*enable)(struct clocksource* cs);
	/**
	 * @brief disable the timer.
	 * @param cs Clocksource to disable.
	 */
	void (*disable)(struct clocksource* cs);
	unsigned long freq; //!< Frequency of the source (in Hz).

	volatile tick_t count; //!< Tick counter.
	tick_t tc_update; //!< Timer update value.

	spinlock_t lock; //!< Clocksource lock.

	struct list_head list; //!< List of clocksources.
	struct timer *thead;
};

/**
 * @struct timer
 * @brief Describes a single (virtual) timer.
 */
struct timer {
	struct timer *next;
	struct timer *prev;

	struct clocksource *source; //!< Source of the timer.
	/**
	 * @brief Timer trigger.
	 * @param timer 'This' timer.
	 * @param arg Argument. Set to priv_data.
	 *
	 * This handle is called everytime tleft hits zero.
	 */
	void (*handle)(struct timer *timer, void *arg);
	void *priv_data; //!< Private timer data.

	unsigned long tleft; //!< Ticks left.
	unsigned long ticks; //!< Total ticks,used for non ONE_SHOT timers.
};

/**
 * @def TIMER_ONESHOT_FLAG
 * @brief Sets the timer to one shot mode.
 *
 * One shot mode means that the timer will only trigger once. After it has
 * triggered it will kill itself.
 */
#define TIMER_ONESHOT_FLAG 0

/**
 * @def TIMER_ONESHOT_MASK
 * @brief Mask for a one shot timer.
 */
#define TIMER_ONESHOT_MASK (1<<TIMER_ONESHOT_FLAG)

/**
 * @def cs_last_update
 * @brief Get the system tick of the last moment \p __cs was updated.
 * @param __cs Clocksource to get the last update timestamp from.
 */
#define cs_last_update(__cs) __cs->tc_update

#define NEVER -1

CDECL
/**
 * @brief Get the tick count from a clock source.
 * @param cs Clock source which tick count is requested.
 * @return The struct clocksource::count attribute of \p cs.
 */
static inline tick_t tm_get_tick(struct clocksource *cs)
{
	tick_t rv;
	unsigned long flags;

	irq_save_and_disable(&flags);
	rv = cs->count;
	irq_restore(&flags);

	return rv;
}

/**
 * @brief Safely increment the tick count of a clock source.
 * @param cs Clock source which tick count is to be incremented.
 * @note The lock of \p cs won't be aquired, only interrupts will be
 * 	 disabled.
 * @note This function does provide overflow protection.
 */
static inline void tm_source_inc(struct clocksource *cs)
{
	unsigned int diff;
	unsigned long flags;

	irq_save_and_disable(&flags);
	if((cs->count + 1UL) == 0) {
		diff = cs->count - cs->tc_update;
		diff += 1;
		cs->tc_update = 0;
		cs->count = diff;
	} else {
		cs->count++;
	}
	irq_restore(&flags);
}

extern tick_t tm_update_source(struct clocksource *source);
extern struct timer *tm_create_timer(struct clocksource *cs, unsigned long ms,
		void (*handle)(struct timer*,void*), void *arg,
		unsigned long flags);
extern int tm_clock_source_initialise(const char *name, struct clocksource *cs,
		unsigned long freq, int (*enable)(struct clocksource *cs),
		void (*disable)(struct clocksource *cs));
extern int tm_stop_timer(struct timer *timer);
extern void tm_process_clock(struct clocksource *cs, unsigned int diff);
extern struct clocksource *tm_get_source_by_name(const char *name);

CDECL_END

#endif /* __TIMER_H__ */
/** @} */

