/*
 *  ETA/OS - GPIO driver
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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

#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <etaos/kernel.h>
#include <etaos/panic.h>

#define ASSERT(__condi__) \
	(__condi) ? (void)0 : panic_P(PSTR("Assert failed in %s:%s at %i"), \
			__FILE__, __func__, __LINE__)

#endif

