/*
 *  ETA/OS - EEPROM header
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
#include <etaos/i2c.h>
#include <etaos/eeprom.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>

static struct eeprom ee_chip;

#define BASE_SLA_24C02 0xA0
#define SCL_FRQ_24C02 100000UL

/**
 * @brief Write a single byte to a 24C02 EEPROM chip.
 * @param addr EEPROM address to write to.
 * @param data Data byte to write to \p addr.
 * @return Error code.
 */
int eeprom_24c02_write_byte(unsigned char addr, unsigned char data)
{
	int rc;
	unsigned char tx[] = { addr, data };

	rc = i2c_master_send(ee_chip.priv, (void*)tx, 2);

	return (rc == 2) ? -EOK : rc;
}

#define MSG_TX 0
#define MSG_RX 1
/**
 * @brief Read a byte from a 24C02 EEPROM chip.
 * @param addr EEPROM address to read from.
 * @param storage Pointer to an address to store the byte read from EEPROM.
 * @return An error code.
 */
int eeprom_24c02_read_byte(unsigned char addr, unsigned char *storage)
{
	int rc;
	unsigned char rx, tx;
	struct i2c_client *client = ee_chip.priv;
	struct i2c_msg msgs[2];

	tx = addr;
	msgs[MSG_TX].dest_addr = client->addr;
	msgs[MSG_TX].flags = 0;
	msgs[MSG_TX].len = 1;
	msgs[MSG_TX].idx = 0;
	msgs[MSG_TX].buff = &tx;

	msgs[MSG_RX].dest_addr = client->addr;
	msgs[MSG_RX].flags = 0;
	msgs[MSG_RX].len = 1;
	msgs[MSG_RX].idx = 0;
	msgs[MSG_RX].buff = &rx;
	set_bit(I2C_RD_FLAG, &msgs[MSG_RX]);

	rc = i2c_bus_xfer(client->bus, msgs, 2);
	*storage = rx;

	return (rc == 2) ? -EOK : rc;
}

/**
 * @brief Initialise an 24C02 EEPROM chip.
 */
void eeprom_init_24c02(void)
{
	struct i2c_device_info *info;

	ee_chip.addr_idx = 0;
	info = i2c_create_info("24C02");
	info->addr = BASE_SLA_24C02;
	ee_chip.priv = i2c_new_device(info);
	kfree(info);

	return;
}

