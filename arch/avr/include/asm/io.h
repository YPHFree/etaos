/*
 *  ETA/OS arch IO
 *  Copyright (C) 2014, 2017   Michel Megens <dev@bietje.net>
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

#ifndef __ARCH_IO_H_
#define __ARCH_IO_H_

#include <asm/config.h>

#ifndef __ASSEMBLER__
#include <asm/init.h>
#endif

#define __noinit __attribute__ ((section (".noinit")))
#define __init3  __attribute__ ((section (".init3")))
#define __naked  __attribute__ ((naked))

#define CPU_CORE_NUM 1

/**
 * @brief 8-bit I/O memory accessor.
 * @param addr Memory address.
 */
#define MEM_IO8(addr) (*(volatile unsigned char*)(addr))
#define MEM_IO16(addr) (*(volatile unsigned short*)(addr))

/**
 * @brief 8-bit I/O memory accessor.
 * @param addr Memory address.
 */
#define IO_ADDR(addr) (*(volatile unsigned char*)(addr))

/**
 * @brief 8-bit I/O memory accessor.
 * @param addr Memory address.
 * @note Used to access the memory below address 0x21, which cannot
 *       be accessed directly.
 */
#define LOW_MEM_IO8(addr) (*(volatile unsigned char*)(addr+0x20))
#define F_CPU CONFIG_FCPU

#ifdef CONFIG_ATMEGA328P
#define CONFIG_ATMEGA328
#endif

#ifdef CONFIG_ATMEGA328
#include <asm/iom328.h>
#endif

#ifdef CONFIG_ATMEGA1280
#include <asm/iom1280.h>
#endif

#ifdef CONFIG_ATMEGA2560
#include <asm/iom1280.h>
#endif

#define irq_enter_critical() __asm__ __volatile__( \
		"in __tmp_reg__, __SREG__" "\n\t" \
		"cli"			   "\n\t" \
		"push __tmp_reg__"	   "\n\t")
#define irq_exit_critical() __asm__ __volatile__( \
		"pop __tmp_reg__"	   "\n\t" \
		"out __SREG__, __tmp_reg__""\n\t")

#ifndef __HARVARD__
#define __HARVARD__ 1
#endif

#endif
