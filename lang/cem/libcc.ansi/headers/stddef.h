/*
 * stddef.h - common definitions
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef _STDDEF_H
#define	_STDDEF_H

#define	NULL 0

#define	offsetof(type, ident)	((size_t) (unsigned long) &((type *)0)->ident)

#if	_EM_PSIZE == _EM_WSIZE
typedef int	ptrdiff_t;	/* result of substracting two pointers */
typedef int intptr_t; /* an integer big enough to store a pointer */
#elif _EM_PSIZE == _EM_LSIZE
typedef long ptrdiff_t;	/* result of substracting two pointers */
typedef long intptr_t; /* an integer big enough to store a pointer */
#else
#error unsupported pointer size
#endif	/* _EM_PSIZE */

typedef unsigned int size_t; /* type returned by sizeof */
typedef char wchar_t; /* type of a wide character */
typedef long off_t; /* type of a position offset */

#endif
