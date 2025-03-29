/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* VARIOUS TARGET MACHINE SIZE DESCRIPTORS */

#include "parameters.h"

#ifndef NOCROSS
extern arith
	short_size, word_size, dword_size, int_size, long_size,
	lnglng_size,
	float_size, double_size, lngdbl_size,
	pointer_size;

extern arith max_int, max_unsigned;	/* cstoper.c	*/
#else /* NOCROSS */
#define short_size	((arith)SZ_SHORT)
#define word_size	((arith)SZ_WORD)
#define dword_size	((arith)2*SZ_WORD)
#define int_size	((arith)SZ_INT)
#define long_size	((arith)SZ_LONG)
#define lnglng_size	((arith)SZ_LNGLNG)
#define float_size	((arith)SZ_FLOAT)
#define double_size	((arith)SZ_DOUBLE)
#define	lngdbl_size	((arith)SZ_LNGDBL)
#define pointer_size	((arith)SZ_POINTER)

#if SZ_INT == 2
#define max_int		((arith)32767)
#define max_unsigned	((arith)65535)
#else /* SZ_INT == 4 */
#define max_int		((arith)2147483647)
#define max_unsigned	((arith)4294967295)
#endif
#endif /* NOCROSS */

