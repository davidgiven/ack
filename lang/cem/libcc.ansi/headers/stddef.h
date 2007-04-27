/*
 * stddef.h - common definitions
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef _STDDEF_H
#define	_STDDEF_H

#include <stdint.h>

#define	NULL 0

#define	offsetof(type, ident)	((size_t) (unsigned long) &((type *)0)->ident)

typedef char wchar_t; /* type of a wide character */
typedef long off_t; /* type of a position offset */

#endif
