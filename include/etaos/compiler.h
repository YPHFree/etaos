/*
 *  Eta/OS - Compiler definitions
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

#ifndef __COMPILER_H__
#define __COMPILER_H__

#ifdef __GNUC__
#include <etaos/compiler-gcc.h>
#endif

#ifndef __used
#define __used
#endif

#ifndef __hot
#define __hot
#endif

#ifndef likely
#define likely(x)
#endif

#ifndef unlikely
#define unlikely(x)
#endif

#endif
