/*
 *  ETA/OS - VFS driver.
 *  Copyright (C) 2012   Michel Megens
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
 * @addtogroup vfs
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/string.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

FILE __iob[MAX_OPEN];
static struct file *vfshead;

/**
 * @brief Add a file to the file descriptor array.
 * @param stream File to add.
 * @return Assigned file descriptor.
 * @retval -1 on error.
 */
int iob_add(FILE stream)
{
	int rc;
		
	for(rc = 3; rc < MAX_OPEN; rc++) {
		if(__iob[rc] == NULL) {
			__iob[rc] = stream;
			stream->fd = rc;
			return rc;
		}
	}
	return -1;
}

/**
 * @brief Remove a file from the FD array.
 * @param fd File descriptor to remove.
 * @retval -EOK on success.
 * @retval -EINVAL on error.
 */
int iob_remove(int fd)
{
	if(__iob[fd]) {
		__iob[fd]->fd = 0;
		__iob[fd] = NULL;
		return -EOK;
	} else {
		return -EINVAL;
	}
}

/**
 * @brief Initialise the VFS.
 */
void vfs_init(void)
{
	int i = 3;

	for(; i < MAX_OPEN; i++)
		__iob[i] = NULL;

	vfshead = NULL;
}

/**
 * @brief Add a file to the virtual file system.
 * @param newfile File to add.
 */
void vfs_add(FILE newfile)
{
	newfile->next = vfshead;
	vfshead = newfile;
}

/**
 * @brief Delete a file from the virtual filesystem.
 * @param file File which has to be deleted.
 * @retval 0 success.
 * @retval -1 on error.
 */
int vfs_delete(FILE file)
{
	struct file **fpp;

	fpp = &file;
	for(; *fpp; fpp = &(*fpp)->next) {
		if(*fpp == file) {
			*fpp = file->next;
			return 0;
		}
	}

	return -1;
}

/**
 * @brief Find a file in the VFS.
 * @param name Name to look for.
 * @return The file pointer.
 * @retval NULL on error (i.e. file not found).
 */
FILE vfs_find(const char *name)
{
	FILE walker;

	for(walker = vfshead; walker; walker = walker->next) {
		if(strcmp(walker->name, name) == 0)
			return walker;
	}

	return NULL;
}
/* @} */

