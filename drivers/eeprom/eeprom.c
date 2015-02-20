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

/**
 * @addtogroup ee
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/eeprom.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/init.h>
#include <etaos/device.h>
#include <etaos/error.h>
#include <etaos/mutex.h>

static inline struct eeprom *to_eeprom_chip(struct vfile * file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	return dev->dev_data;
}

/**
 * @brief Write to an EEPROM chip.
 * @param stream File stream.
 * @param buf Buffer with data to write.
 * @param len Length of \p buf.
 */
static int eeprom_write(struct vfile * stream, const void *buf, size_t len)
{
	int rc;
	struct eeprom *ee;

	if(stream)
		ee = to_eeprom_chip(stream);
	else
		return -EINVAL;

	if(!ee->write)
		return -EINVAL;

	mutex_lock(&ee->lock);
	rc = ee->write(ee, buf, len);
	ee->wr_idx += len;
	mutex_unlock(&ee->lock);

	return rc;
}

/**
 * @brief Read from an EEPROM chip.
 * @param stream File stream.
 * @param buf Buffer to read data into.
 * @param len Length of \p buf.
 */
static int eeprom_read(struct vfile * stream, void *buf, size_t len)
{
	int rc;
	struct eeprom *ee;

	if(stream)
		ee = to_eeprom_chip(stream);
	else
		return -EINVAL;

	if(!ee->read)
		return -EINVAL;

	mutex_lock(&ee->lock);
	rc = ee->read(ee, buf, len);
	ee->rd_idx += len;
	mutex_unlock(&ee->lock);

	return rc;
}

/**
 * @brief Write a single byte to an EEROM chip.
 * @param c Byte to write.
 * @param stream File stream.
 * @return Error code.
 */
static int eeprom_put(int c, struct vfile * stream)
{
	int rc;
	struct eeprom *ee;

	if(stream)
		ee = to_eeprom_chip(stream);
	else
		return -EINVAL;

	if(!ee->write_byte)
		return -EINVAL;

	mutex_lock(&ee->lock);
	rc = ee->write_byte(ee, c);
	ee->wr_idx++;
	mutex_unlock(&ee->lock);

	return rc;
}

/**
 * @brief Read a single byte from an EEPROM chip.
 * @param stream File stream.
 * @return The byte read from the chip or an error code.
 */
static int eeprom_get(struct vfile * stream)
{
	int rc;
	struct eeprom *ee;

	if(stream)
		ee = to_eeprom_chip(stream);
	else
		return -EINVAL;

	if(!ee->read_byte)
		return -EINVAL;

	mutex_lock(&ee->lock);
	rc = ee->read_byte(ee);
	ee->rd_idx++;
	mutex_unlock(&ee->lock);

	return rc;
}

/**
 * @brief EEPROM core IOCTL.
 * @param stream File stream.
 * @param reg Control register.
 * @param buf Optional argument for reg.
 * 
 * Available control options are found in eeprom_ioctl_t.
 */
static int eeprom_ioctl(struct vfile * stream, unsigned long reg, void *buf)
{
	int rc;
	unsigned long idx;
	struct eeprom *ee;

	if(!stream)
		return -EINVAL;

	ee = to_eeprom_chip(stream);

	if(!buf)
		idx = 0;
	else
		idx = *((unsigned long*)buf);

	mutex_lock(&ee->lock);
	switch(reg) {
	case EEPROM_RESET_WR_IDX:
		ee->wr_idx = idx;
		rc = -EOK;
		break;
		
	case EEPROM_RESET_RD_IDX:
		ee->rd_idx = idx;
		rc = -EOK;
		break;

	default:
		rc = -EINVAL;
		break;
	}
	mutex_unlock(&ee->lock);

	return rc;
}

static struct dev_file_ops eeprom_ops = {
	.write = &eeprom_write,
	.read = &eeprom_read,
	.put = &eeprom_put,
	.get = &eeprom_get,
	.ioctl = &eeprom_ioctl,
};

/**
 * @brief Initialise a new EEPROM chip driver.
 * @param ee EEPROM chip descriptor which should be initialised.
 * @param dev Device descriptor for \p ee.
 * @note ee::name should be initialised when calling this function.
 */
void eeprom_chip_init(struct eeprom *ee, struct device *dev)
{
	ee->wr_idx = 0;
	ee->rd_idx = 0;
	mutex_init(&ee->lock);

	if(dev) {
		dev->name = ee->name;
		device_initialize(dev, &eeprom_ops);
		dev->dev_data = ee;
	}
}

static void __used eeprom_init(void)
{
}

subsys_init(eeprom_init);

/** @} */

