/*
 *  ETA/OS - I2C client
 *  Copyright (C) 2014   Michel Megens
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
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/types.h>
#include <etaos/i2c.h>
#include <etaos/list.h>
#include <etaos/device.h>
#include <etaos/bitops.h>

int i2c_master_send(const struct i2c_client *client, const char *buf, int count)
{
	int ret;
	struct i2c_bus *bus;
	struct i2c_msg msg;

	bus = client->bus;
	msg.dest_addr = client->addr;
	msg.flags = 0;
	msg.len = count;
	msg.idx = 0;
	msg.buff = (char*)buf;

	ret = i2c_bus_xfer(bus, &msg, 1);

	return (ret == 1) ? count : -EINVAL;
}

int i2c_master_recv(const struct i2c_client *client, char *buf, int count)
{
	struct i2c_bus *bus = client->bus;
	struct i2c_msg msg;
	int ret;

	msg.dest_addr = client->addr;
	set_bit(I2C_RD_FLAG, &msg.flags);
	msg.len = count;
	msg.buff = buf;
	msg.idx = 0;

	ret = i2c_bus_xfer(bus, &msg, 1);

	return (ret == 1) ? count : -EINVAL;
}

struct i2c_client *i2c_new_device(struct i2c_device_info *info)
{
	struct i2c_client *client;
	struct i2c_bus *bus;

	if(!info)
		return NULL;

	client = kzalloc(sizeof(*client));
	if(!client)
		return NULL;

	client->dev.name = info->name;
	client->addr = info->addr;
	if(info->bus)
		bus = info->bus;
	else
		bus = i2c_sysbus;
	
	device_initialize(&client->dev, &info->fops);
	i2c_add_client(bus, client);

	return client;
}

