/*
 *  ETA/OS - STDIO streams of the USART
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
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/usart.h>
#include <etaos/error.h>

#include <asm/io.h>
#include <asm/usart.h>

int atmega_usart_putc(struct usart *usart, int c)
{
	if(c == '\n')
		atmega_usart_putc(usart, '\r');

	while(( UCSR0A & BIT(UDRE0) ) == 0);
	UCSR0A |= BIT(TXCn);
	UDR0 = c;

	return c;
}

static int atmega_usart_write(struct usart *uart, const void *tx,
			size_t txlen)
{
	int err;
	size_t i;
	const char *txbuf = tx;

	for(i = 0; i < txlen; i++) {
		err = atmega_usart_putc(uart, txbuf[i]);
		if(err != txbuf[i]) {
			err = -EINVAL;
			break;
		}
	}
	
	if(err < 0)
		return 0;
	else
		return err;
}

static struct usart atmega_usart = {
	.putc = atmega_usart_putc,
	.write = atmega_usart_write,
	.timeout = 0,
	.dev = { .name = "atm-usart", },
};

void atmega_usart_init(void)
{
	UBRR0H = UBRR0H_VALUE;
	UBRR0L = UBRR0L_VALUE;
	UCSR0A &= ~(BIT(U2X0));
	UCSR0C = BIT(UCSZ01) | BIT(UCSZ00);
	UCSR0B = BIT(TXEN0);

	mutex_init(&(atmega_usart.bus_lock));
	usart_initialise(&atmega_usart);
}

