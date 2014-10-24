/*
 *  ETA/OS - I2C bus
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
#include <etaos/types.h>
#include <etaos/i2c.h>
#include <etaos/list.h>
#include <etaos/bitops.h>
#include <etaos/time.h>
#include <etaos/tick.h>

static int __i2c_transfer(struct i2c_bus *bus,
		struct i2c_msg msgs[], int len)
{
	int ret;
	char retries;
	int64_t orig_tick;

	orig_tick = sys_tick;
	for(retries = 0, ret = 0; retries < bus->retries; retries++) {
		ret = bus->xfer(bus, msgs, retries);

		if(ret != -EAGAIN)
			break;
		if(time_after(orig_tick, orig_tick + bus->timeout))
			break;
	}

	return ret;
}

int i2c_bus_xfer(struct i2c_bus *bus, 
			struct i2c_msg msgs[], int len)
{
	int ret;

	mutex_lock(&bus->lock);
	ret = __i2c_transfer(bus, msgs, len);
	mutex_unlock(&bus->lock);

	return (ret == -EOK) ? len : ret;
}

int i2c_master_send(const struct i2c_client *client, const char *buf, int count)
{
	int ret;
	struct i2c_bus *bus;
	struct i2c_msg msg;

	bus = client->bus;
	msg.dest_addr = client->addr;
	msg.flags = 0;
	msg.len = count;
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

	ret = i2c_bus_xfer(bus, &msg, 1);

	return (ret == 1) ? count : -EINVAL;
}

int i2c_init_bus(struct i2c_bus *bus)
{
	if(!bus)
		return -EINVAL;

	mutex_init(&bus->lock);
	list_head_init(&bus->clients);
	return -EOK;
}

void i2c_add_client(struct i2c_bus *bus, struct i2c_client *client)
{
	if(!bus || !client)
		return;

	list_add(&client->list_entry, &bus->clients);
	client->bus = bus;
}

