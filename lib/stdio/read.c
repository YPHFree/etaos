/*
 *  ETA/OS - LibC read
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
#include <etaos/stdio.h>
#include <etaos/bitops.h>

/**
 * @ingroup libc
 * @brief Read from a file.
 * @param fd File descriptor to read from.
 * @param buff Buffer to store data into.
 * @param size Size of \p buff.
 */
int read(int fd, void *buff, size_t size)
{
	struct vfile * file;

	file = __iob[fd];
	if(!file)
		return -EINVAL;

	if(test_bit(STREAM_READ_FLAG, &file->flags) && file->read)
		return file->read(file, buff, size);
	else
		return -EINVAL;
}

