#
#   ETA/OS - SRAM library
#   Copyright (C) 2017  Michel Megens <dev@bietje.net>
#   Copyright (C) 2017  Dean Hall
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

## @addtogroup python-sys
# @{

## @package sys
#  @brief Provides PyMite's system module, sys
#
#  USAGE
#  -----
#
#  import sys
#

"""__NATIVE__
#include <etaos/thread.h>
"""

maxint = 0x7FFFFFFF     # 2147483647


## Stop the program.
# @param val Exit code.
# @return None.
def exit(val):
    """__NATIVE__
    pPmObj_t pval = C_NULL;
    PmReturn_t retval;

    /* If no arg given, assume return 0 */
    if (NATIVE_GET_NUM_ARGS() == 0)
    {
        NATIVE_SET_TOS(PM_ZERO);
    }

    /* If 1 arg given, put it on stack */
    else if (NATIVE_GET_NUM_ARGS() == 1)
    {
        pval = NATIVE_GET_LOCAL(0);
        NATIVE_SET_TOS(pval);
    }

    /* If wrong number of args, raise TypeError */
    else
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Raise the SystemExit exception */
    PM_RAISE(retval, PM_RET_EX_EXIT);
    return retval;
    """
    pass


## Runs the Garbage Collector
# @return None.
def gc():
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
#ifdef HAVE_GC
    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 0)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    retval = heap_gcRun();
#endif
    NATIVE_SET_TOS(PM_NONE);

    return retval;
    """
    pass


## Gets a byte from the platform's default I/O
# @return Byte obtained from stdin.
def getb():
    """__NATIVE__
    uint8_t b;
    pPmObj_t pb;
    PmReturn_t retval;

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 0)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    retval = plat_getByte(&b);
    PM_RETURN_IF_ERROR(retval);

    retval = int_new((int32_t)b, &pb);
    NATIVE_SET_TOS(pb);
    return retval;
    """
    pass


## Returns a tuple containing the amout of heap available and the maximum
# @return Tuple object.
def heap():
    """__NATIVE__
    PmReturn_t retval;
    pPmObj_t pavail;
    pPmObj_t psize;
    pPmObj_t ptup;
    uint8_t objid;

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 0)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Allocate a tuple to store the return values */
    retval = tuple_new(2, &ptup);
    PM_RETURN_IF_ERROR(retval);

    /* Get the maximum heap size */
    heap_gcPushTempRoot(ptup, &objid);
    retval = int_new(heap_getSize(), &psize);
    if (retval != PM_RET_OK)
    {
        heap_gcPopTempRoot(objid);
        return retval;
    }

    /* Allocate an int to hold the amount of heap available */
    retval = int_new(heap_getAvail() - sizeof(PmInt_t), &pavail);
    heap_gcPopTempRoot(objid);
    PM_RETURN_IF_ERROR(retval);

    /* Put the two heap values in the tuple */
    ((pPmTuple_t)ptup)->val[0] = pavail;
    ((pPmTuple_t)ptup)->val[1] = psize;

    /* Return the tuple on the stack */
    NATIVE_SET_TOS(ptup);

    return retval;
    """
    pass


## Sends the LSB of the integer out the platform's default I/O
# @param b Byte to sent to stdout.
# @return None.
def putb(b):
    """__NATIVE__
    uint8_t b;
    pPmObj_t pb;
    PmReturn_t retval;

    pb = NATIVE_GET_LOCAL(0);

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* If arg is not an int, raise TypeError */
    if (OBJ_GET_TYPE(pb) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    b = ((pPmInt_t)pb)->val & 0xFF;
    retval = plat_putByte(b);
    NATIVE_SET_TOS(PM_NONE);
    return retval;
    """
    pass


## Runs the given function in a thread sharing the current global namespace
# @param f Function object.
# @return None.
def run(f):
	"""__NATIVE__
	PmReturn_t retval;
	pPmObj_t pf;

	/* If wrong number of args, raise TypeError */
	if (NATIVE_GET_NUM_ARGS() != 1) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	/* If arg is not a function, raise TypeError */
	pf = NATIVE_GET_LOCAL(0);
	if (OBJ_GET_TYPE(pf) != OBJ_TYPE_FXN) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	retval = interp_addThread((pPmFunc_t)pf);
	NATIVE_SET_TOS(PM_NONE);
	return retval;
	"""
	pass


## Returns the number of milliseconds since the PyMite VM was initialized
# @return Number of milliseconds since VM initialization.
def clock():
    """__NATIVE__
    uint32_t t;
    pPmObj_t pt;
    PmReturn_t retval;

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 0)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get the system time (milliseconds since init) */
    retval = plat_getMsTicks(&t);
    PM_RETURN_IF_ERROR(retval);

    /*
     * Raise ValueError if there is an overflow
     * (plat_getMsTicks is unsigned; int is signed)
     */
    if ((int32_t)t < 0)
    {
        PM_RAISE(retval, PM_RET_EX_VAL);
        return retval;
    }

    /* Return an int object with the time value */
    retval = int_new((int32_t)t, &pt);
    NATIVE_SET_TOS(pt);
    return retval;
    """
    pass

## Yield the PythonVM.
# @return None.
def thread_yield():
	"""__NATIVE__
	yield();
	return PM_RET_OK;
	"""
	pass

## Waits in a busy loop for the given number of milliseconds
# @param ms Number milliseconds to wait.
# @return None.
#
def wait(ms):
	t = clock() + ms
	while clock() < t:
		pass

## @}

# :mode=c:
