/*
 *  ETA/OS - USART driver header
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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
 * @file etaos/usart.h
 */

/**
 * @addtogroup usart-core
 */
/*@{*/

#ifndef __USART_H__
#define __USART_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/device.h>
#include <etaos/mutex.h>

/**
 * @struct usart
 * @brief USART descriptor.
 */
struct usart {
	int timeout; //!< Transmission timeout.
	mutex_t rx, tx;

	struct device dev; //!< Backend USART device.
	/**
	 * @brief Write to the USART device.
	 * @param uart USART to write to.
	 * @param tx TX buffer.
	 * @param txlen Length of \p tx.
	 * @return Error code.
	 */
	int (*write)(struct usart *uart, const void *tx,
			size_t txlen);
	/**
	 * @brief Write a single byte to the USART device.
	 * @param uart USART to write to.
	 * @param c Character to write.
	 * @return The written character.
	 */
	int (*putc)(struct usart* uart, int c);
	/**
	 * @brief Read a single byte from the USART.
	 * @param uart USART to read from.
	 * @return The byte read.
	 */
	int (*getc)(struct usart* uart);
	/**
	 * @brief Read from a USART device.
	 * @param rx RX buffer.
	 * @param len Length of \p rx.
	 * @return Error code.
	 */
	int (*read)(struct usart *uart, void *rx, size_t len);
};

CDECL

extern int usart_initialise(struct usart *usart);
extern void setup_usart_streams(struct usart *uart);

CDECL_END

#endif

/* @} */

