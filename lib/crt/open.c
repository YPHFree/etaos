/*
 *  ETA/OS - VFS open
 *  Copyright (C) 2012, 2016   Michel Megens <dev@bietje.net>
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

/**
 * @addtogroup libcio
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

/**
 * @ingroup vfs
 * @brief Open a file on the virtual file system.
 * @param name File name to look for.
 * @param flags File flags.
 * @return The file descriptor. If an error occurs -EINVAL is returned.
 */
int open(const char *name, unsigned long flags)
{
	return vfs_open(name, flags);
}

/** @} */

