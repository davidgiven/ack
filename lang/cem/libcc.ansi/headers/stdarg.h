/*
 * stdarg.h - variable arguments
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef	_STDARG_HEADER_
#define	_STDARG_HEADER_

typedef	char	*va_list;

#define	va_start(ap, parmN)	(ap = (char *)&parmN)
#define	va_arg(ap, type)	((type *)(ap += sizeof(type)))[0]
#define	va_end(ap)

#endif	/* _STDARG_HEADER_ */
