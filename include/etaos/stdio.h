/*
 *  Eta/OS - ETA/OS stdio header
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

#ifndef __STDIO_H__
#define __STDIO_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>

/**
 * \def MAX_OPEN
 * \brief Maximum amount of files opened at the same time.
 */
#define MAX_OPEN 16

/**
 * @name File flags
 */
/* @{ */
#define __SRD	0x0001 //!< OK to read
#define __SWR	0x0002 //!< OK to write
#define __SRWB  0x0004 //!< Read/write from buffer
#define __SPGM	0x0008 //!< fmt string is in progmem
#define __SERR	0x0010 //!< found error
#define __SEOF	0x0020 //!< found EOF
#define __SUNGET 0x040 //!< ungetc() happened
#define __SMALLOC 0x80 //!< handle is malloc()ed
/* @} */

#define STREAM_READ_FLAG 	0
#define STREAM_WRITE_FLAG 	1
#define STREAM_RW_BUFFER_FLAG 	2
#define STREAM_FMT_PROGMEM_FLAG 3
#define STREAM_ERROR_FLAG 	4
#define STREAM_EOF_FLAG 	5
#define STREAM_UNGETC_FLAG 	6
#define STREAM_MALLOC_FLAG 	7

#define _FDEV_SETUP_READ  __SRD	/**< fdev_setup_stream() with read intent */
#define _FDEV_SETUP_WRITE __SWR	/**< fdev_setup_stream() with write intent */
#define _FDEV_SETUP_RW    (__SRD|__SWR)	/**< fdev_setup_stream() with read/write intent */
#define _FDEV_SETUP_RWB   __SRWB /**< Read/write from buffers */

#define EOF 1

struct vfile;
/**
 * \brief Define a file stream.
 * \param defname Variable name of the stream.
 * \param r Read function pointer.
 * \param w Write function pointer.
 * \param p Put function pointer.
 * \param g Get function pointer.
 * \param f Flush function pointer.
 * \param n File name function pointer.
 * \param fl Flags byte.
 * \param d Data pointer.
 * \warning The file name (<b>n</b>) MUST be unique.
 * 
 * This defines an initialized file stream structure.
 */
#define FDEV_SETUP_STREAM(defname, r, w, p, g, f, n, fl, d) \
	struct vfile defname = {	\
	.write = r,		\
	.read = w,		\
	.put = p,		\
	.get = g,		\
	.flush = f,		\
	.name = n,		\
	.flags = fl,		\
	.data = d,		\
	 /* default initializations */ \
	.length = 0,		\
	.index = 0,		\
	.buff = NULL,		\
}

/**
 * @ingroup vfs
 * @brief File definition
 */
struct vfile {
	const char *name; //!< File name.
	struct vfile *next; //!< Next file.
	spinlock_t lock;

	unsigned long flags; //!< File flags.
	int fd; //!< Assigned file descriptor.

	int (*open)(struct vfile*); //!< Open a file.
	int (*close)(struct vfile*); //!< File close.
	int (*read)(struct vfile*, void*, size_t); //!< Read from a file.
	int (*write)(struct vfile*, const void*, size_t); //!< Write to a file.
	int (*flush)(struct vfile*); //!< Flush the file.
	int (*put)(int c, struct vfile*); //!< Write 1 byte to a file.
	int (*get)(struct vfile*); //!< Read 1 byte from a file.
	int (*ioctl)(struct vfile*, unsigned long reg, void *buf);

	void *data; //!< Private file data.
	volatile unsigned char *buff; //!< File buffer.
	size_t length; //!< Length of buff.
	size_t index; //!< Index in buff.

};

CDECL

extern struct vfile * __iob[];

#define stdin 	__iob[0]
#define stdout 	__iob[1]
#define stderr 	__iob[2]

#define filep(__idx) __iob[__idx]

extern int putc(int c, struct vfile * stream);
extern int fputc(int c, struct vfile * stream);
extern int fputs(char *c, struct vfile * stream);
extern int fprintf(struct vfile * stream, const char*, ...);
extern int printf(const char *, ...);
extern int vfprintf(struct vfile * stream, const char *fmt, va_list va);
extern int iob_add(struct vfile * iob);
extern int iob_remove(int fd);
extern void close(int fd);
extern int open(const char *name, unsigned long flags);
extern int write(int fd, const void *buff, size_t size);
extern int read(int fd, void *buff, size_t size);
extern int ioctl(struct vfile *stream, unsigned long reg, void *buf);
extern int getc(struct vfile *stream);
extern int fgetc(struct vfile *stream);

CDECL_END

#endif

