/*
 *  Eta/OS - AVR bitops
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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

#ifndef __AVR_BITOPS_H_
#define __AVR_BITOPS_H_

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>

#define BITS_PER_LONG 32UL
#define BITS_PER_BYTE  8UL

static inline void clear_bit(unsigned nr, volatile unsigned long *flags)
{
	unsigned char *p = ((unsigned char*)flags) + (nr / BITS_PER_BYTE);
	unsigned char msk = 1UL << (nr % BITS_PER_BYTE);
	unsigned char tmp;

	__asm__ __volatile__(
			"ld %0, %a2"	"\n\t"
			"com %3"	"\n\t"
			"and %0, %3"	"\n\t"
			"st %a1, %0"	"\n\t"
			: "=&d" (tmp), "=e" (p)
			: "e" (p), "d" (msk)
			: "memory"
			);
}

static inline void set_bit(unsigned nr, volatile unsigned long *addr)
{
	unsigned char *p = ((unsigned char*)addr) + (nr / BITS_PER_BYTE);
	unsigned char msk = 1UL << (nr % BITS_PER_BYTE);
	unsigned char tmp;

	__asm__ __volatile__(
			"ld %0, %a2"	"\n\t"
			"or %0, %3"	"\n\t"
			"st %a1, %0"	"\n\t"
			: "=&d" (tmp), "=e" (p)
			: "e" (p), "d" (msk)
			: "memory"
			);
}

static inline int test_and_clear_bit(unsigned nr, volatile unsigned long *addr)
{
	unsigned char *p = ((unsigned char*)addr) + (nr / BITS_PER_BYTE);
	unsigned char msk = 1UL << (nr % BITS_PER_BYTE);
	unsigned char tmp, old;

	__asm__ __volatile__(
			"ld %0, %a3"	"\n\t"
			"mov %2, %0"	"\n\t"
			"and %2, %4"	"\n\t"
			"com %4"	"\n\t"
			"and %0, %4"	"\n\t"
			"st %a1, %0"	"\n\t"
			: "=&r" (tmp), "=e" (p), "=&r" (old)
			: "e" (p), "d" (msk)
			: "memory"
			);
	return old != 0;
}

static inline int test_and_set_bit(unsigned nr, volatile unsigned long *addr)
{
	unsigned char *p = ((unsigned char*)addr) + (nr / BITS_PER_BYTE);
	unsigned char msk = 1UL << (nr % BITS_PER_BYTE);
	unsigned char tmp, old;

	__asm__ __volatile__(
			"ld %0, %a3"	"\n\t"
			"mov %2, %0"	"\n\t"
			"and %2, %4"	"\n\t"
			"or %0, %4"	"\n\t"
			"st %a1, %0"	"\n\t"
			: "=&r" (tmp), "=e" (p), "=&r" (old)
			: "e" (p), "d" (msk)
			: "memory"
			);
	return old != 0;
}

#endif
