/*
 *  ETA/OS - Python header
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
 *  Copyright (C) 2017   Dean Hall
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

#ifndef __PM_H__
#define __PM_H__

/**
 * \file
 * \brief PyMite Header
 *
 * Include things that are needed by nearly everything.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_SNPRINTF_FORMAT
#include <etaos/stdio.h>
#endif

#include <etaos/types.h>
#include <etaos/limits.h>

/**
 * Value indicating the release of PyMite
 *
 * This value should be incremented for every public release.
 * It helps locate a defect when used in conjunction with a fileID
 * and line number.
 */
#define PM_RELEASE 8

/** null for C++ and C code */
#ifdef __cplusplus
#define C_NULL (0)
#else
#define C_NULL ((void *)0)
#endif

/** false for C code */
#define C_FALSE (uint8_t)0

/** true for C code */
#define C_TRUE (uint8_t)1

/** Comparison result is that items are the same */
#define C_SAME (int8_t)0

/** Comparison result is that items differ */
#define C_DIFFER (int8_t)-1

/** PORT inline for C code */
#define INLINE __inline__

/**
 * Returns an exception error code and stores debug data
 *
 * This macro must be used as an rval statement.  That is, it must
 * be used after an assignment such as "retval = " or a return statement
 */
#if __DEBUG__
#define PM_RAISE(retexn, exn) \
        do \
        { \
            retexn = (exn); \
            gVmGlobal.errFileId = __FILE_ID__; \
            gVmGlobal.errLineNum = (uint16_t)__LINE__; \
        } while (0)
#else
#define PM_RAISE(retexn, exn) \
        retexn = (exn)
#endif

/** if retval is not OK, break from the block */
#define PM_BREAK_IF_ERROR(retval) if ((retval) != PM_RET_OK) break

/** return an error code if it is not PM_RET_OK */
#define PM_RETURN_IF_ERROR(retval)  if ((retval) != PM_RET_OK) return (retval)

/** print an error message if argument is not PM_RET_OK */
#define PM_REPORT_IF_ERROR(retval)   if ((retval) != PM_RET_OK) \
                                        plat_reportError(retval)

/** Jumps to a label if argument is not PM_RET_OK */
#define PM_GOTO_IF_ERROR(retval, target) if ((retval) != PM_RET_OK) \
                                            goto target

#if __DEBUG__
/** If the boolean expression fails, return the ASSERT error code */
#define C_ASSERT(boolexpr) \
    do \
    { \
        if (!((boolexpr))) \
        { \
            gVmGlobal.errFileId = __FILE_ID__; \
            gVmGlobal.errLineNum = (uint16_t)__LINE__; \
            return PM_RET_ASSERT_FAIL; \
        } \
    } \
    while (0)

#else
/** Assert statements are removed from production code */
#define C_ASSERT(boolexpr)
#endif

/** Use as the first argument to C_DEBUG_PRINT for low volume messages */
#define VERBOSITY_LOW 1

/** Use as the first argument to C_DEBUG_PRINT for medium volume messages */
#define VERBOSITY_MEDIUM 2

/** Use as the first argument to C_DEBUG_PRINT for high volume messages */
#define VERBOSITY_HIGH 3

#if __DEBUG__
#include <etaos/stdio.h>

/** To be used to set DEBUG_PRINT_VERBOSITY to a value so no prints occur */
#define VERBOSITY_OFF 0

/** Sets the level of verbosity to allow in debug prints */
#define DEBUG_PRINT_VERBOSITY VERBOSITY_OFF

/** Prints a debug message when the verbosity is within the set value */
#define C_DEBUG_PRINT(v, f, ...) \
    do \
    { \
        if (DEBUG_PRINT_VERBOSITY >= (v)) \
        { \
            printf("PM_DEBUG: " f, ## __VA_ARGS__); \
        } \
    } \
    while (0)

#else
#define C_DEBUG_PRINT(...)
#endif

/**
 * Return values for system functions
 * to report status, errors, exceptions, etc.
 * Normally, functions which use these values
 * should propagate the same return value
 * up the call tree to the interpreter.
 */
	typedef enum PmReturn_e {
		/* general status return values */
		PM_RET_OK = 0,	/**< Everything is ok */
		PM_RET_NO = 0xFF,
				/**< General "no result" */
		PM_RET_ERR = 0xFE,
				/**< General failure */
		PM_RET_STUB = 0xFD,
				/**< Return val for stub fxn */
		PM_RET_ASSERT_FAIL = 0xFC,
				/**< Assertion failure */
		PM_RET_FRAME_SWITCH = 0xFB,
				/**< Frame pointer was modified */
		PM_RET_ALIGNMENT = 0xFA,
				/**< Heap is not aligned */

		/* return vals that indicate an exception occured */
		PM_RET_EX = 0xE0,
				/**< General exception */
		PM_RET_EX_EXIT = 0xE1,
				/**< System exit */
		PM_RET_EX_IO = 0xE2,
				/**< Input/output error */
		PM_RET_EX_ZDIV = 0xE3,
				/**< Zero division error */
		PM_RET_EX_ASSRT = 0xE4,
				/**< Assertion error */
		PM_RET_EX_ATTR = 0xE5,
				/**< Attribute error */
		PM_RET_EX_IMPRT = 0xE6,
				/**< Import error */
		PM_RET_EX_INDX = 0xE7,
				/**< Index error */
		PM_RET_EX_KEY = 0xE8,
				/**< Key error */
		PM_RET_EX_MEM = 0xE9,
				/**< Memory error */
		PM_RET_EX_NAME = 0xEA,
				/**< Name error */
		PM_RET_EX_SYNTAX = 0xEB,
				/**< Syntax error */
		PM_RET_EX_SYS = 0xEC,
				/**< System error */
		PM_RET_EX_TYPE = 0xED,
				/**< Type error */
		PM_RET_EX_VAL = 0xEE,
				/**< Value error */
		PM_RET_EX_STOP = 0xEF,
				/**< Stop iteration */
		PM_RET_EX_WARN = 0xF0,
				/**< Warning */
		PM_RET_EX_OFLOW = 0xF1,
				/**< Overflow */
	} PmReturn_t;

	extern volatile uint32_t pm_timerMsTicks;

/* WARNING: The order of the following includes is critical */
#include <asm/python/pm.h>
#include <asm/python/features.h>

#include <etaos/python/pmEmptyPlatformDefs.h>
#include <etaos/python/sli.h>
#include <etaos/python/mem.h>
#include <etaos/python/obj.h>
#include <etaos/python/seq.h>
#include <etaos/python/tuple.h>
#include <etaos/python/strobj.h>
#include <etaos/python/heap.h>
#include <etaos/python/int.h>
#include <etaos/python/seglist.h>
#include <etaos/python/list.h>
#include <etaos/python/dict.h>
#include <etaos/python/codeobj.h>
#include <etaos/python/func.h>
#include <etaos/python/module.h>
#include <etaos/python/frame.h>
#include <etaos/python/class.h>
#include <etaos/python/interp.h>
#include <etaos/python/img.h>
#include <etaos/python/global.h>
#include <etaos/python/thread.h>
#include <etaos/python/float.h>
#include <etaos/python/cpu.h>
#include <etaos/python/eeprom.h>
#include <etaos/python/sram.h>
#include <etaos/python/device.h>
#include <etaos/python/plat_interface.h>
#include <etaos/python/bytearray.h>

/** Pointer to a native function used for lookup tables in interp.c */
	typedef PmReturn_t(*pPmNativeFxn_t) (pPmFrame_t *);
	extern pPmNativeFxn_t const std_nat_fxn_table[];
	extern pPmNativeFxn_t const usr_nat_fxn_table[];

/**
 * Initializes the PyMite virtual machine and indexes the user's application
 * image.  The VM heap and globals are reset.  The argument, pusrimg, may be
 * null for interactive sessions.
 *
 * @param heap_base The address where the contiguous heap begins
 * @param heap_size The size in bytes (octets) of the given heap.
 *             Must be a multiple of four.
 * @param memspace      Memory space in which the user image is located
 * @param pusrimg       Address of the user image in the memory space
 * @return Return status
 */
	PmReturn_t pm_init(uint8_t * heap_base, uint32_t heap_size,
			   PmMemSpace_t memspace, uint8_t const *const pusrimg);

/**
 * Executes the named module
 *
 * @param modstr        Name of module to run
 * @return Return status
 */
	PmReturn_t pm_run(uint8_t const *modstr);

/**
 * Needs to be called periodically by the host program.
 * For the desktop target, it is periodically called using a signal.
 * For embedded targets, it needs to be called periodically. It should
 * be called from a timer interrupt.
 *
 * @param usecsSinceLastCall Microseconds (not less than those) that passed
 *                           since last call. This must be <64535.
 * @return Return status
 */
	PmReturn_t pm_vmPeriodic(uint16_t usecsSinceLastCall);

#ifdef __cplusplus
}
#endif
#endif				/* __PM_H__ */
