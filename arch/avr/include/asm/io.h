/*
 *  ETA/OS arch IO
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

#ifndef __ARCH_IO_H_
#define __ARCH_IO_H_

#include <asm/config.h>

#define BIT(__x) (1 << (__x))
#define MEM_IO8(addr) (*(volatile unsigned char*)(addr))
#define F_CPU CONFIG_FCPU

#ifdef CONFIG_ATMEGA328P
#define CONFIG_ATMEGA328
#endif

#ifdef CONFIG_ATMEGA328
#include <asm/iom328.h>
#endif

#define irq_enter_critical() __asm__ __volatile__( \
		"in __tmp_reg__, __SREG__" "\n\t" \
		"cli"			   "\n\t" \
		"push __tmp_reg__"	   "\n\t")
#define irq_exit_critical() __asm__ __volatile__( \
		"pop __tmp_reg__"	   "\n\t" \
		"out __SREG__, __tmp_reg__""\n\t")

#endif

