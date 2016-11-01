/*
 *  ETA/OS - Standard definitions
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

#ifndef __STDDEF_H__
#define __STDDEF_H__

#ifdef __GNUCLIKE_BUILTIN_VARARGS
typedef __builtin_va_list	__va_list;	/* internally known to gcc */
#else
typedef	char *			__va_list;
#endif /* __GNUCLIKE_BUILTIN_VARARGS */
#if defined(__GNUC_VA_LIST_COMPATIBILITY) && !defined(__GNUC_VA_LIST) \
	    && !defined(__NO_GNUC_VA_LIST)
#define __GNUC_VA_LIST
typedef __va_list		__gnuc_va_list;	/* compatibility w/GNU headers*/
#endif

#ifndef _VA_LIST_DECLARED
#define	_VA_LIST_DECLARED
typedef	__va_list	va_list;
#endif

#ifdef __GNUCLIKE_BUILTIN_STDARG

#define	va_start(ap, last) \
		__builtin_va_start((ap), (last))

#define	va_arg(ap, type) \
		__builtin_va_arg((ap), type)

#if __ISO_C_VISIBLE >= 1999
#define	va_copy(dest, src) \
		__builtin_va_copy((dest), (src))
#endif

#define	va_end(ap) \
		__builtin_va_end(ap)

#else	/* !__GNUCLIKE_BUILTIN_STDARG */

#define	__va_size(type) \
		(((sizeof(type) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#ifdef __GNUCLIKE_BUILTIN_NEXT_ARG
#define va_start(ap, last) \
		((ap) = (va_list)__builtin_next_arg(last))
#else	/* !__GNUCLIKE_BUILTIN_NEXT_ARG */
#define	va_start(ap, last) \
		((ap) = (va_list)&(last) + __va_size(last))
#endif	/* __GNUCLIKE_BUILTIN_NEXT_ARG */

#define	va_arg(ap, type) \
		(*(type *)((ap) += __va_size(type), (ap) - __va_size(type)))

#if __ISO_C_VISIBLE >= 1999
#define	va_copy(dest, src) \
		((dest) = (src))
#endif

#define	va_end(ap)

#endif /* __GNUCLIKE_BUILTIN_STDARG */

#endif /* __STDDEF_H__ */

