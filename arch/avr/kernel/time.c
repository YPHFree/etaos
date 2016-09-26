/*
 *  ETA/OS - AVR time
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
 * @addtogroup atmega
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/timer.h>
#include <etaos/tick.h>
#include <etaos/delay.h>
#include <etaos/init.h>

#include <asm/time.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/cpu.h>

/**
 * @def AVR_SYSCLK_FRQ
 * @brief Frequency of the AVR system clock.
 */
#define AVR_SYSCLK_FRQ 1000UL

static struct clocksource sysclk = {
	.name = "sys-clk",
};

/**
 * @brief Getter for the ATmega AVR system clock.
 * @return The ATmega AVR system clock.
 * @see avr_start_sysclk
 */
struct clocksource *avr_get_sys_clk(void)
{
	return &sysclk;
}

/**
 * @brief Enable the AVR system clock.
 * @param cs Clocksource which has to be enabled.
 * @note Dummy function, the AVR sysclk is enabled on initialise.
 */
static int avr_sysclk_enable(struct clocksource *cs)
{
	return -EOK;
}

/**
 * @brief Start the AVR timers.
 * @see avr_sysclk_enable avr_start_sysclk
 */
static void __used avr_timer_init(void)
{
	clocksource_init(sysclk.name, &sysclk, AVR_SYSCLK_FRQ,
					&avr_sysclk_enable, NULL);
	avr_start_sysclk(TIMER0_OVERFLOW_VECTOR_NUM, &sysclk);
	sysctl(SYS_SET_SYSCLK, &sysclk);
}

#define delay_loop2(__count) \
	__asm__ __volatile__( \
		"1: sbiw %0,1" "\n\t" \
		"brne 1b" \
		: "=w" (__count) \
		: "0" (__count) \
	);

#define delay_loop(__count) \
	__asm__ volatile ( \
		"1: dec %0" "\n\t" \
		"brne 1b" \
		: "=r" (__count) \
		: "0" (__count) \
	);

#ifdef CONFIG_DELAY_US
void arch_delay_us(double __us)
{
	double __tmp;
	uint16_t __ticks;
	double __tmp2;

	__tmp = ((F_CPU) / 3e6) * __us;
	__tmp2 = ((F_CPU) / 4e6) * __us;
	if (__tmp < 1.0) {
		__ticks = 1;
	}
	else if (__tmp2 > 65535) {
		delay(__us / 1000);
		return;
	} else if (__tmp > 255) {
		__ticks = (uint16_t)__tmp2;
		delay_loop2(__ticks);
		return;
	} else {
		__ticks = (uint8_t)__tmp;
	}

	delay_loop(__ticks);
}
#endif

/**
 * @brief Start the AVR system clock.
 * @param irq IRQ vector number.
 * @param src Clocksource structure for the AVR sysclk.
 */
void avr_start_sysclk(int irq, struct clocksource *src)
{
	systick_setup(irq, src);
#if F_CPU == 16000000
	OCR0A = 250;
#elif F_CPU == 8000000
	OCRA0 = 125;
#else
#error Unsupported CPU frequency for timer IRQ
#endif
	TIMSK0 = TOIE0;
	TCCR0A = WGM00 | WGM01;
	TCCR0B = WGM02 | CS00 | CS01;
}

extern void preempt_schedule(void);
SIGNAL(TIMER0_OVERFLOW_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();

#ifdef CONFIG_IRQ_DEBUG
	test_sys_tick++;

	if((test_sys_tick % 5000) == 0)
		chip->chip_handle(EXT_IRQ0_NUM);
#endif

	chip->chip_handle(TIMER0_OVERFLOW_VECTOR_NUM);

#ifdef CONFIG_PREEMPT
	__asm__ __volatile__(
			"sei"	"\n\t"
			:
			:
			: "memory"
			);
	preempt_schedule();
	__asm__ __volatile__(
			"cli"	"\n\t"
			:
			:
			: "memory"
			);
#endif
}

subsys_init(avr_timer_init);

/* @} */

