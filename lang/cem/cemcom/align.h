/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	 A L I G N M E N T   D E F I N I T I O N S	*/

#include "nofloat.h"

extern int
	short_align, word_align, int_align, long_align,
#ifndef NOFLOAT
	float_align, double_align,
#endif NOFLOAT
	pointer_align,
	struct_align, union_align;

extern arith align();
