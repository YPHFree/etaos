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
 * @file kernel/time/timer.c
 * @addtogroup tm
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/list.h>
#include <etaos/list.h>
#include <etaos/error.h>
#include <etaos/timer.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/spinlock.h>
#include <etaos/atomic.h>
#include <etaos/tick.h>


/**
 * @brief Start a new timer.
 * @param timer Timer to start.
 */
static void timer_start_timer(struct timer *timer)
{
	struct clocksource *cs;

	cs = timer->source;
	clocksource_add_timer(cs, timer);
}

/**
 * @brief Create a new timer.
 * @param cs Clock source handling the timer.
 * @param ms Timer interval in miliseconds.
 * @param handle Timer handle.
 * @param arg Private timer data (passed to handle on trigger).
 * @param flags Timer flags.
 * @return The created time, NULL if no timer was created.
 */
struct timer *timer_create_timer(struct clocksource *cs, unsigned long ms,
		void (*handle)(struct timer*,void*), void *arg,
		unsigned long flags)
{
	struct timer *timer;

	if((timer = kzalloc(sizeof(*timer))) == NULL)
		return NULL;

	timer->tleft = (cs->freq / 1000UL) * ms;

	if(test_bit(TIMER_ONESHOT_FLAG, &flags))
		timer->ticks = 0;
	else
		timer->ticks = timer->tleft;

	timer->tleft += clocksource_get_diff(cs);
	timer->source = cs;
	timer->handle = handle;
	timer->priv_data = arg;

	timer_start_timer(timer);
	return timer;
}	

/**
 * @brief Stop a timer.
 * @param timer Timer to stop.
 * @return Error code.
 * @retval 0 on success.
 * @retval 1 if no timer was stopped.
 */
int timer_stop_timer(struct timer *timer)
{
	unsigned long flags;
	struct clocksource *cs = timer->source;

	if(!timer || timer == SIGNALED)
		return -1;

	raw_spin_lock_irqsave(&cs->lock, flags);
	timer->ticks = 0;
	timer->handle = NULL;
	timer->tleft = 0;
	raw_spin_unlock_irqrestore(&cs->lock, flags);

	if(timer->tleft) {
		clocksource_delete_timer(cs, timer);
		timer_start_timer(timer);
	}

	return 0;
}

extern void raw_clocksource_add_timer(struct clocksource *cs, struct timer *timer);
/**
 * @brief Process a given clock source and its timers.
 * @param cs Clock source to process.
 * @param diff Time difference in ticks since last call to this function.
 * @warning This function should NOT be called by applications.
 */
void timer_process_clock(struct clocksource *cs, unsigned int diff)
{
	struct timer *timer;
	
	_raw_spin_lock(&cs->lock);
	if(!cs->thead || diff < 0 || diff == 0) {
		_raw_spin_unlock(&cs->lock);
		return;
	}

	while(cs->thead && diff) {
		timer = cs->thead;

		if(diff < timer->tleft) {
			timer->tleft -= diff;
			diff = 0;
		} else {
			diff -= timer->tleft;
			timer->tleft = 0;
		}

		if(timer->tleft == 0) {
			if(timer->handle) {
				timer->handle(timer, timer->priv_data);
			}

			cs->thead = cs->thead->next;
			if(cs->thead)
				cs->thead->prev = NULL;

			if((timer->tleft = timer->ticks) == 0) {
				kfree(timer);
			} else {
				raw_clocksource_add_timer(cs, timer);
			}
		}
	}
	_raw_spin_unlock(&cs->lock);
}

/* TIMER REWRITE BELOW */

static inline bool timer_is_expired(struct timer *timer)
{
	time_t now;

	now = clocksource_get_tick(timer->source);
	return time_at_or_after(now, timer->expire_at);
}

static int timer_list_comparator(struct list_head *lh1, struct list_head *lh2)
{
	struct timer *t1, *t2;

	t1 = list_entry(lh1, struct timer, entry);
	t2 = list_entry(lh2, struct timer, entry);

	if(t1->expire_at < t2->expire_at)
		return -1;
	else if(t1->expire_at > t2->expire_at)
		return 1;

	return 0;
}

struct timer *timer_create(struct clocksource *cs, unsigned long ms,
		void (*handle)(struct timer*,void*), void *arg,
		unsigned long flags)
{
	struct timer *timer;
	time_t expire;


	if((timer = kzalloc(sizeof(*timer))) == NULL)
		return NULL;

	timer->interval = expire = (cs->freq / 1000UL) * ms;
	expire += clocksource_get_tick(cs);

	if(test_bit(TIMER_ONESHOT_FLAG, &flags))
		timer->interval = 0;

	list_head_init(&timer->entry);
	timer->expire_at = expire;
	timer->source = cs;
	timer->handle = handle;
	timer->priv_data = arg;

	clocksource_insert_timer(cs, &timer->entry, &timer_list_comparator);
	return timer;
}

int timer_stop(struct timer *timer)
{
	int rc;

	rc = clocksource_remove_timer(timer->source, &timer->entry);
	if(rc == -EOK)
		kfree(timer);

	return rc;
}

void timer_process(struct clocksource *cs)
{
	unsigned long flags;
	struct list_head *carriage, *x;
	struct timer *timer;

	raw_spin_lock_irqsave(&cs->lock, flags);
	clocksource_update(cs);

	list_for_each_safe(carriage, x, &cs->timers) {
		timer = list_entry(carriage, struct timer, entry);
		if(timer_is_expired(timer)) {
			list_del(carriage);
			
			if(timer->handle)
				timer->handle(timer, timer->priv_data);

			if(timer->interval) {
				timer->expire_at = timer->interval +
					clocksource_get_tick(cs);
				raw_clocksource_insert_timer(cs, &timer->entry,
						&timer_list_comparator);
			} else {
				kfree(timer);
			}

		}
	}

	raw_spin_unlock_irqrestore(&cs->lock, flags);
}

/** @} */
