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
#define __FILE_ID__ 0x17

/**
 * \file
 * \brief Float Object Type
 *
 * Float object type operations.
 */

#include <etaos/python.h>

#ifdef HAVE_FLOAT
#include <etaos/math.h>

PmReturn_t float_new(float f, pPmObj_t * r_pf)
{
	PmReturn_t retval = PM_RET_OK;

	retval = heap_getChunk(sizeof(PmFloat_t), (uint8_t **) r_pf);
	PM_RETURN_IF_ERROR(retval);
	OBJ_SET_TYPE(*r_pf, OBJ_TYPE_FLT);
	((pPmFloat_t) * r_pf)->val = f;
	return retval;
}

#ifdef HAVE_PRINT

PmReturn_t float_print(pPmObj_t pf)
{
	uint8_t tBuffer[32];
	uint8_t bytesWritten;
	uint8_t *p;
	PmReturn_t retval = PM_RET_OK;

	C_ASSERT(pf != C_NULL);

	/* Raise TypeError if obj is not an float */
	if (OBJ_GET_TYPE(pf) != OBJ_TYPE_FLT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	/* #212: Use homebrew float formatter */
	retval = sli_ftoa(((pPmFloat_t) pf)->val, tBuffer, sizeof(tBuffer));
	bytesWritten = sli_strlen((char *)tBuffer);

	/* Remove trailing zeroes (per Python convention) */
	for (p = &tBuffer[bytesWritten] - 1;
	     p[0] == '0' && p[-1] != '.'; --p, bytesWritten--) ;
	++p;
	*p = '\0';

	/* Sanity check */
	C_ASSERT(bytesWritten != 0);
	C_ASSERT(bytesWritten < sizeof(tBuffer));

	sli_puts(tBuffer);
	return PM_RET_OK;
}

PmReturn_t float_negative(pPmObj_t pf, pPmObj_t * r_pf)
{
	/* Create new int obj */
	return float_new(-((pPmFloat_t) pf)->val, r_pf);
}

#endif				/* HAVE_PRINT */

PmReturn_t float_op(pPmObj_t px, pPmObj_t py, pPmObj_t * r_pn, int8_t op)
{
	float x;
	float y;
	float r;
	PmReturn_t retval;

	/* Raise TypeError if args aren't ints or floats */
	if (((OBJ_GET_TYPE(px) != OBJ_TYPE_INT)
	     && (OBJ_GET_TYPE(px) != OBJ_TYPE_FLT))
	    || ((OBJ_GET_TYPE(py) != OBJ_TYPE_INT)
		&& (OBJ_GET_TYPE(py) != OBJ_TYPE_FLT))) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	/* Get the values as floats */
	if (OBJ_GET_TYPE(px) == OBJ_TYPE_INT) {
		x = (float)((pPmInt_t) px)->val;
	} else {
		x = ((pPmFloat_t) px)->val;
	}

	if (OBJ_GET_TYPE(py) == OBJ_TYPE_INT) {
		y = (float)((pPmInt_t) py)->val;
	} else {
		y = ((pPmFloat_t) py)->val;
	}

	/* Raise ZeroDivisionError if denominator is zero */
	if ((y == 0.0) && ((op == '/') || (op == '%'))) {
		PM_RAISE(retval, PM_RET_EX_ZDIV);
		return retval;
	}

	/* Calculate x raised to y */
	switch (op) {
        /* *INDENT-OFF* */
        case '+': r = x + y; break;
        case '-': r = x - y; break;
        case '*': r = x * y; break;
        case '/': r = x / y; break;
        case '%': r = fmodf(x, y); break;
        case 'P': r = powf(x, y); break;
        default: r = 0.0; break;
        /* *INDENT-ON* */
	}

	retval = float_new(r, r_pn);

	return retval;
}

PmReturn_t
float_compare(pPmObj_t px, pPmObj_t py, pPmObj_t * r_pobj, PmCompare_t cmp)
{
	float x;
	float y;
	PmReturn_t retval = PM_RET_OK;
	int8_t t8 = 0;

	/* Raise TypeError if args aren't ints or floats */
	if (((OBJ_GET_TYPE(px) != OBJ_TYPE_INT)
	     && (OBJ_GET_TYPE(px) != OBJ_TYPE_FLT))
	    || ((OBJ_GET_TYPE(py) != OBJ_TYPE_INT)
		&& (OBJ_GET_TYPE(py) != OBJ_TYPE_FLT))) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	/* Get the values as floats */
	if (OBJ_GET_TYPE(px) == OBJ_TYPE_INT) {
		x = (float)((pPmInt_t) px)->val;
	} else {
		x = ((pPmFloat_t) px)->val;
	}

	if (OBJ_GET_TYPE(py) == OBJ_TYPE_INT) {
		y = (float)((pPmInt_t) py)->val;
	} else {
		y = ((pPmFloat_t) py)->val;
	}

	switch (cmp) {
        /* *INDENT-OFF* */
        case COMP_LT: t8 = (int8_t)(x <  y); break;
        case COMP_LE: t8 = (int8_t)(x <= y); break;
        case COMP_EQ: t8 = (int8_t)(x == y); break;
        case COMP_NE: t8 = (int8_t)(x != y); break;
        case COMP_GT: t8 = (int8_t)(x >  y); break;
        case COMP_GE: t8 = (int8_t)(x >= y); break;
        case COMP_IS: t8 = (int8_t)(px == py); break;
        case COMP_IS_NOT: t8 = (int8_t)(px != py);break;
        case COMP_IN:
        case COMP_NOT_IN:
            PM_RAISE(retval, PM_RET_EX_TYPE);
            break;

        default:
            /* Other compares are not yet supported */
            PM_RAISE(retval, PM_RET_EX_SYS);
            break;
        /* *INDENT-ON* */
	}
	*r_pobj = (t8) ? PM_TRUE : PM_FALSE;

	return retval;
}

#endif				/* HAVE_FLOAT */
