/*
 *  ETA/OS - Python VM
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

#undef __FILE_ID__
#define __FILE_ID__ 0x15

/**
 * \file
 * \brief PyMite User API
 *
 * High-level functions to initialize and run PyMite
 */

#include <etaos/python.h>

/** Number of millisecond-ticks to pass before scheduler is run */
#define PM_THREAD_TIMESLICE_MS  5

/** Stores the timer millisecond-ticks since system start */
volatile uint32_t pm_timerMsTicks = 0;

/** Stores tick timestamp of last scheduler run */
volatile uint32_t pm_lastRescheduleTimestamp = 0;

PmReturn_t
pm_init(uint8_t * heap_base, uint32_t heap_size,
	PmMemSpace_t memspace, uint8_t const *const pusrimg)
{
	PmReturn_t retval;

	/* Initialize the hardware platform */
	retval = plat_init();
	PM_RETURN_IF_ERROR(retval);

	/* Initialize the heap and the globals */
	retval = heap_init(heap_base, heap_size);
	PM_RETURN_IF_ERROR(retval);

	retval = global_init();
	PM_RETURN_IF_ERROR(retval);

	/* Load usr image info if given */
	if (pusrimg != C_NULL) {
		retval = img_appendToPath(memspace, pusrimg);
	}

	return retval;
}

PmReturn_t pm_run(uint8_t const *modstr)
{
	PmReturn_t retval;
	pPmObj_t pmod;
	pPmObj_t pstring;
	uint8_t const *pmodstr = modstr;
	uint8_t objid1;
	uint8_t objid2;

	/* Import module from global struct */
	retval = string_new(&pmodstr, &pstring);
	PM_RETURN_IF_ERROR(retval);
	heap_gcPushTempRoot(pstring, &objid1);
	retval = mod_import(pstring, &pmod);
	PM_RETURN_IF_ERROR(retval);

	/* Load builtins into thread */
	heap_gcPushTempRoot(pmod, &objid2);
	retval = global_setBuiltins((pPmFunc_t) pmod);
	PM_RETURN_IF_ERROR(retval);

	/* Interpret the module's bcode */
	retval = interp_addThread((pPmFunc_t) pmod);
	PM_RETURN_IF_ERROR(retval);
	heap_gcPopTempRoot(objid1);
	retval = interpret(INTERP_RETURN_ON_NO_THREADS);

	/*
	 * De-initialize the hardware platform.
	 * Ignore plat_deinit's retval so interpret's retval returns to caller.
	 */
	plat_deinit();

	return retval;
}

/* Warning: Can be called in interrupt context! */
PmReturn_t pm_vmPeriodic(uint16_t usecsSinceLastCall)
{
	/*
	 * Add the full milliseconds to pm_timerMsTicks and store additional
	 * microseconds for the next run. Thus, usecsSinceLastCall must be
	 * less than 2^16-1000 so it will not overflow usecResidual.
	 */
	static uint16_t usecResidual = 0;

	C_ASSERT(usecsSinceLastCall < 64536);

	usecResidual += usecsSinceLastCall;
	while (usecResidual >= 1000) {
		usecResidual -= 1000;
		pm_timerMsTicks++;
	}

	/* Check if enough time has passed for a scheduler run */
	if ((pm_timerMsTicks - pm_lastRescheduleTimestamp)
	    >= PM_THREAD_TIMESLICE_MS) {
		interp_setRescheduleFlag((uint8_t) 1);
		pm_lastRescheduleTimestamp = pm_timerMsTicks;
	}
	return PM_RET_OK;
}
