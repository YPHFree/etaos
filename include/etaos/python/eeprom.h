/*
 *  ETA/OS - EEPROM header
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

#ifndef __PYTHON_EEPROM_H__
#define __PYTHON_EEPROM_H__

#include <etaos/kernel.h>

CDECL
extern int pm_ee_read(uint8_t addr, void *buff, size_t len);
extern int pm_ee_write(uint8_t addr, const void *buff, size_t len);
CDECL_END

#endif

