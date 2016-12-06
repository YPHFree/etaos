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

/**
 * @brief Directory entry descriptor.
 */
struct dirent {
	char *name; //!< Directory name.

	struct dirent *parent; //!< Parent directory.
	struct list_head entry; //!< List entry.
	struct list_head children; //!< List head of child directory's.

	struct file *file_head; //!< List of file entry's.
	struct fs_driver *fs;    //!< File system driver.
};

CDECL

extern void vfs_init(void);

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
extern size_t lseek(struct file *file, size_t offset, int whence);

extern int mkdir(const char *path);
extern int mount(struct fs_driver *fs, const char *path);
extern int unlink(const char *path);
extern struct dirent *opendir(const char *dirname);
extern struct dirent *readdir(struct dirent *dirp);
extern struct dirent *readdir_r(struct dirent *dir, struct dirent *entry,
		struct dirent **result);

/* DIRENT functions */
extern struct dirent *dirent_create(const char *name);
extern struct dirent *dirent_find(struct dirent *root, const char *path);
extern struct dirent *dirent_add_child(struct dirent *parent,
		struct dirent *child);
extern struct file *dirent_add_file(struct dirent *dir, struct file *file);
extern struct file *dirent_remove_file(struct dirent *dir, struct file *file);
extern struct file *dirent_find_file(struct dirent *dir, const char *filename);

CDECL_END

#endif

