/*
 *  Eta/OS - ETA/OS kernel header
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

#ifndef __ETAOS_KERNEL_H__
#define __ETAOS_KERNEL_H__

#include <generated/autoconf.h>
#include <etaos/types.h>
#include <etaos/stddef.h>
#include <etaos/compiler.h>

#define NULL ((void*)0)
#define SIGNALED ((void*)-1)
#define ERR_PTR ((void*)-1)
#define FALSE 0
#define TRUE !FALSE

#define true TRUE
#define false FALSE

#ifdef __compiler_offsetof
#define offsetof(TYPE, MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({		\
		const typeof( ((type *)0)->member) *__mptr = (ptr); \
		(type *)( ( char *)__mptr - offsetof(type,member) );})

#endif

