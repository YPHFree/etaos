/*
 *  ETA/OS - Clock sources
 *  Copyright (C) 2014, 2015, 2016   Michel Megens
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
 * @file kernel/time/clocksource.c
 * @addtogroup tm
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/list.h>
#include <etaos/error.h>
#include <etaos/clocksource.h>
#include <etaos/time.h>
#include <etaos/string.h>

static struct list_head sources = STATIC_INIT_LIST_HEAD(sources);

/**
 * @brief initialise a new clock source.
 * @param name Name of the clocksource.
 * @param cs Clock source which has to be initialised.
 * @param freq Frequency of the clock source.
 * @param enable Function pointer to enable the source.
 * @param disable Function pointer to disable the source.
 * @return An error code.
 */
int clocksource_init(const char *name, struct clocksource *cs,
		unsigned long freq, int (*enable)(struct clocksource *cs),
		void (*disable)(struct clocksource *cs))
{
	if(!cs || !name)
		return -EINVAL;

	cs->thead = NULL;
	cs->name = name;
	cs->freq = freq;
	cs->enable = enable;
	cs->disable = disable;
	cs->count = 0UL;
	cs->tc_update = 0UL;
	spinlock_init(&cs->lock);
	list_add(&cs->list, &sources);
	list_head_init(&cs->timers);
	return -EOK;
}

/**
 * @brief Get the clock count of a clocksource in seconds.
 * @param src Clock to get the tick count of.
 * @return The clock count in seconds of \p src.
 */
time_t clocksource_get_seconds(struct clocksource *src)
{
	time_t tick;

	tick = clocksource_get_tick(src);
	tick /= src->freq;

	return tick;
}

/**
 * @brief Get a timer source based on its name.
 * @param name Name to search for.
 * @return The clock source, if found.
 * @retval NULL if no clocksource was found.
 */
struct clocksource *clocksource_get_by_name(const char *name)
{
	struct clocksource *cs;
	struct list_head *carriage;

	list_for_each(carriage, &sources) {
		cs = list_entry(carriage, struct clocksource, list);
		if(!strcmp(name, cs->name))
			return cs;
	}

	return NULL;
}

void raw_clocksource_add_timer(struct clocksource *cs, struct timer *timer)
{
	struct timer *_timer;

	for(_timer = cs->thead; _timer; _timer = _timer->next) {
		if(timer->tleft < _timer->tleft) {
			_timer->tleft -= timer->tleft;
			break;
		}

		timer->tleft -= _timer->tleft;
		timer->prev = _timer;
	}

	timer->next = _timer;
	if(timer->next)
		timer->next->prev = timer;
	if(timer->prev)
		timer->prev->next = timer;
	else
		cs->thead = timer;
}

/**
 * @brief Add a new timer to a clocksource.
 * @param cs Clocksource to add \p timer to.
 * @param timer Timer to add.
 */
void clocksource_add_timer(struct clocksource *cs, struct timer *timer)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&cs->lock, flags);
	raw_clocksource_add_timer(cs, timer);
	raw_spin_unlock_irqrestore(&cs->lock, flags);
}

/**
 * @brief Update the clockskew of a clocksource.
 * @param cs Clocksource to update.
 * @return The clockskew of \p source.
 * @note You should call timer_process_clock as soon as possible after calling
 *       this function to keep timers up to date.
 */
tick_t clocksource_update(struct clocksource *cs)
{
	tick_t diff;

	diff = clocksource_get_diff(cs);
	cs->tc_update = clocksource_get_tick(cs);
	return diff;
}

/**
 * @brief Remove a timer from a clocksource.
 * @param cs Clocksource to remove \p timer from.
 * @param timer Timer to remove.
 */
void clocksource_delete_timer(struct clocksource *cs, struct timer *timer)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&cs->lock, flags);
	if(timer->prev)
		timer->prev->next = timer->next;
	else
		cs->thead = timer->next;
	if(timer->next) {
		timer->next->prev = timer->prev;
		timer->next->tleft += timer->tleft;
	}
	raw_spin_unlock_irqrestore(&cs->lock, flags);
}

/* TIMER REWRITE */

void raw_clocksource_insert_timer(struct clocksource *cs,
		struct list_head *lh, list_comparator_t comp)
{
	struct list_head *carriage;

	if(list_empty(&cs->timers)) {
		list_add(lh, &cs->timers);
		return;
	}

	list_for_each(carriage, &cs->timers) {
		if(list_is_last(carriage, &cs->timers)) {
			list_add(lh, carriage);
			break;
		}

		if(comp(lh, carriage) <= 0) {
			list_add_tail(lh, carriage);
			break;
		}
	}
}

void clocksource_insert_timer(struct clocksource *cs, struct list_head *lh,
				list_comparator_t comp)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&cs->lock, flags);
	raw_clocksource_insert_timer(cs, lh, comp);
	raw_spin_unlock_irqrestore(&cs->lock, flags);
}

int clocksource_remove_timer(struct clocksource *cs, struct list_head *lh)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&cs->lock, flags);
	list_del(lh);
	raw_spin_unlock_irqrestore(&cs->lock, flags);

	return -EOK;
}

/** @} */

