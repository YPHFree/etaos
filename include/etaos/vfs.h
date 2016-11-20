/*
 *  ETA/OS - VFS driver.
 *  Copyright (C) 2012, 2014, 2015   Michel Megens
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
 * @file include/etaos/vfs.h VFS header
 */

#ifndef __VFS_H__
#define __VFS_H__

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/list.h>

/**
 * @ingroup vfs
 * @brief Seek operations type.
 */
typedef enum {
	SEEK_SET, //!< Set the index to the input.
	SEEK_CUR, //!< Set the index to the current index + input.
	SEEK_END, //!< Set the index to the file end + input.
} lseek_t;

struct dirent {
	char *name;
	struct vfile *device;

	struct list_head entry;
	struct list_head children;

	struct vfile *file_head;
	struct fs_driver *fs;
};

CDECL

extern void vfs_init(void);
extern void vfs_add(struct vfile * file);
extern int vfs_delete(struct vfile * f);
extern struct vfile * vfs_find(const char *name);

extern int vfs_read(struct vfile *file, void *buff, size_t size);
extern int vfs_write(struct vfile *file, const void *buff, size_t size);

extern ssize_t vfs_setoffset(struct vfile *file, ssize_t offset, ssize_t max);
extern size_t lseek(struct vfile *file, size_t offset, int whence);

CDECL_END

#endif

