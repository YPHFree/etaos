/*
 *  Eta/OS - AVR5 arch boot
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

#include <etaos/kernel.h>
#include <etaos/bitops.h>
#include <etaos/stdio.h>

#include <asm/io.h>
#include <asm/simulavr.h>

extern void avr_init(void);

extern unsigned char __heap_start;

void avr_init(void)
{
#ifdef CONFIG_SIMUL_AVR
	simul_avr_setup_streams();
#endif
	//simul_avr_write_string("Booting!\n", NULL);
	printf("Booting!\n");

	while(1);
}
