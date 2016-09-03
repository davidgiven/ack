/*
 * stdarg.h - variable arguments
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef _STDARG_H
#define	_STDARG_H

#if defined(__PCC__)

/* This only works on pcc architectures. */

typedef __builtin_va_list va_list;
#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v)      __builtin_va_end(v)
#define va_arg(v, l)   __builtin_va_arg(v, l)

#else

/* ...and this only works on EM architectures. */

typedef char* va_list;

#define __vasz(x)		((sizeof(x)+sizeof(int)-1) & ~(sizeof(int) -1))

#define va_start(ap, parmN)	(ap = (va_list)&parmN + __vasz(parmN))
#define va_arg(ap, type)	(*((type *)(void *)((ap += __vasz(type)) - __vasz(type))))
#define va_end(ap)

#endif

#endif
