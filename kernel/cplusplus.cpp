/*
 *  ETA/OS - C++ support
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
#include <etaos/types.h>
#include <etaos/mem.h>

/**
 * @brief Create a new object and call the constructor.
 * @param num Number of bytes to allocate.
 * @return Allocated memory for the new object.
 */
void *operator new(size_t num)
{
	return kmalloc(num);
}

/**
 * @brief Free the memory of an allocated object.
 * @param ptr Memory region to free.
 */
void operator delete(void *ptr)
{
	if(!ptr)
		kfree(ptr);
}

/**
 * @brief Lock for data initialisation.
 * @param g Lock to aquire.
 * @return Non zero if the lock was aquired succesfully.
 */
int __cxa_guard_acquire(__guard *g) 
{
	return !*(char *)(g);
}

/**
 * @brief Release the lock aquired with __cxa_guard_acquire
 */
void __cxa_guard_release (__guard *g) 
{
	*(char *)g = 1;
}

/**
 * @brief Temporary implementation for pure virtual functions.
 * @todo Implement some big time complaining and add a panic.
 */
void __cxa_pure_virtual(void)
{
	/*
	 * TODO: Do some big time complaining here
	 */
}

/**
 * @brief Abort the program.
 */
void __cxa_guard_abort (__guard *g)
{
}

