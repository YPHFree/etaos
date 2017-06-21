/*
 *  ETA/OS - Memory module
 *  Copyright (C) 2014, 2016   Michel Megens <dev@bietje.net>
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

/** @file etaos/mem.h */

#ifndef __MEM_H__
#define __MEM_H__

/**
 * @addtogroup mm
 * @{
 */
#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>

#ifdef CONFIG_MM_TRACE_OWNER
#include <etaos/thread.h>
#endif

#define MM_MAGIC_BYTE 0x99

/**
 * @brief Heap node structure
 */
struct heap_node {
	size_t size; //!< Size of the node.
#ifdef CONFIG_MM_TRACE_OWNER
	struct thread *owner; //!< Owner of the memory region.
#endif
	struct heap_node *next; //!< Heap node list pointer.
};

#define MM_ALLOC_FLAG 0 //!< This bit becomes set after allocation

#define MEM __attribute__((malloc))

CDECL
#ifdef CONFIG_MM_DEBUG
extern int mm_free(void*, const char *, int);
#define kfree(__p) mm_free(__p, __FILE__, __LINE__)
#else
extern int mm_free(void*);
extern void kfree(void *);
#endif

extern void mm_init(void);
extern void mm_heap_add_block(void *start, size_t size);
extern void raw_mm_heap_add_block(void *start, size_t size);
extern size_t mm_node_size(void *ptr);

extern MEM void* mm_alloc(size_t);
extern MEM void *mm_alloc_aligned(size_t size, size_t alignment);

extern void *kzalloc(size_t num);
extern void *kmalloc(size_t num);
extern void *kcalloc(size_t, size_t);
extern void *krealloc(void *old, size_t newsize);

extern size_t mm_heap_available(void);
CDECL_END

/** @} */
#endif

