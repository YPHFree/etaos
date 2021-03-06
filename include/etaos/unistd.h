/*
 *  ETA/OS - Unistd header
 *  Copyright (C) 2016  Michel Megens <dev@bietje.net>
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

#ifndef __UNISTD_H__
#define __UNISTD_H__

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

CDECL
extern size_t lseek(struct file *file, size_t offset, int whence);
extern int mkdir(const char *path);
extern int mount(struct fs_driver *fs, const char *path);
extern int unlink(const char *path);
CDECL_END

#endif /* __UNISTD_H__ */
