/*
 *  ETA/OS - VFS driver.
 *  Copyright (C) 2012, 2014, 2015, 2016   Michel Megens <dev@bietje.net>
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
#include <etaos/dirent.h>

/**
 * @ingroup vfs
 * @brief Seek operations type.
 */
typedef enum {
	SEEK_SET, //!< Set the index to the input.
	SEEK_CUR, //!< Set the index to the current index + input.
	SEEK_END, //!< Set the index to the file end + input.
} lseek_t;

/**
 * @brief File system control
 */
typedef enum {
	FS_FILE_OPEN, //!< File opened.
	FS_FILE_READ, //!< File read occurred.
	FS_FILE_WRITE, //!< File write occurred.
	FS_FILE_CLOSE, //!< File closed.
	FS_FILE_UNLINK, //!< Remove a file.

	FS_DIR_CREATE, //!< Directory created.
	FS_DIR_OPEN,   //!< Directory opened.
	FS_DIR_READ,   //!< Directory read occurred.

} fs_ctrl_t;

/**
 * @brief File system driver.
 *
 * Structure describing how a file system operates.
 */
struct fs_driver {
	struct file *(*open)(const char *path, int mode); //!< Open a file.
	int (*close)(struct file*); //!< File close.
	int (*read)(struct file*, void*, size_t); //!< Read from a file.
	int (*write)(struct file*, const void*, size_t); //!< Write to a file.
	int (*flush)(struct file*); //!< Flush the file.
	int (*put)(int c, struct file*); //!< Write 1 byte to a file.
	int (*get)(struct file*); //!< Read 1 byte from a file.

	/**
	 * @brief I/O control function pointer.
	 */
	int (*ioctl)(struct file*, unsigned long reg, void *buf);
};

extern spinlock_t vfs_lock;
extern struct dirent vfs_root;

CDECL

extern void vfs_init(void);

extern struct file *raw_vfs_init_buffered_file(struct file *f,
		size_t n, void *buf);
extern struct file *vfs_create_buffered_file(size_t bufsize);
extern struct file *vfs_init_buffered_file(struct file *f, size_t bufsize);
extern struct file *vfs_find_file(const char *path);
extern struct fs_driver *vfs_path_to_fs(const char *path);
extern int vfs_add_file(const char *path, struct file *file);
extern int vfs_open(const char *path, int mode);
extern int vfs_close(int fd);

extern int vfs_read(struct file *file, void *buff, size_t size);
extern int vfs_write(struct file *file, const void *buff, size_t size);
extern int vfs_ioctl(struct dirent *dir, struct file *file,
		unsigned long reg, void *arg);

extern ssize_t vfs_setoffset(struct file *file, ssize_t offset, ssize_t max);
extern size_t vfs_setindex(struct file *file, size_t index, size_t max);

CDECL_END

#endif

