/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* VARIOUS TARGET MACHINE SIZE DESCRIPTORS */

#include "nofloat.h"

extern arith
	short_size, word_size, dword_size, int_size, long_size,
#ifndef NOFLOAT
	float_size, double_size,
#endif NOFLOAT
	pointer_size;

extern arith max_int, max_unsigned;	/* cstoper.c	*/
