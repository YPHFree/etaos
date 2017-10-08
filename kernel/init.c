/*
 *  ETA/OS - Init
 *  Copyright (C) 2014, 2015  Michel Megens
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
#include <etaos/init.h>
#include <etaos/types.h>
#include <etaos/sched.h>
#include <etaos/thread.h>
#include <etaos/mem.h>

bool post_early_init = false;
bool post_dev_init   = false;
bool post_init       = false;

#ifdef weak_sym
weak_sym void finalize_init(void)
{
}
#endif

extern int main(void);
void kinit(void)
{
#ifdef CONFIG_SCHED
	sched_start();
#else
	/* normally the scheduler enables the interrupts, we don't have a
	 * scheduler to take care of us so we have to do it ourselves.
	 */
	irq_enable();
	finalize_init();
	main();
	while(true);
#endif
}

#ifdef CONFIG_SCHED
void main_thread_func(void *arg)
{
	finalize_init();
	main();
	while(true);
}
#endif
