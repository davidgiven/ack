/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S H A R E D   F I L E
 *
 *  C S E T . C
 */


#include "types.h"
#include "cset.h"
#include "alloc.h"
#include "debug.h"
#include "global.h"


/* A set over a range of integers from 1 to N may be represented
 * as a 'compact' set. Such a set is represented as a 'bitvector'
 * record, containing the size of the set (i.e. N) and a row
 * of words (the bitvector itself). An integer J (1 <= J  <= N) is
 * an element of the set iff the J-th bit of the vector is a '1'.
 * Any redundant bits in the last word are garanteed to be zero bits.
 * This package implements the usual operations on sets.
 * The name of every operation is preceede by a 'C' to
 * distinguish it from the operation on 'long' (list) 
 * sets whth a similar name.
 */


/* The two arithmetic operations 'divide by wordlength' and
 * 'modulo wordlength' can be performed very efficiently
 * if the word length (of the source machine) is 16.
 */




cset Cempty_set(n)
	short n;
{
	cset s;

	s = newbitvect(DIVWL(n-1) + 1);
	s->v_size = n;
	return s;
}


bool Cis_elem(x,s)
	Celem_t x;
	cset    s;
{
	short n;
	int mask;

	assert(x>0 && x <= s->v_size);
	n = DIVWL(x-1);
	mask = (1 << MODWL(x-1));
	if ((s->v_bits[n] & mask) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}



Cadd(x,s_p)
	Celem_t x;
	cset    *s_p;
{
	cset s;
	short n;
	int mask;

	s = *s_p;
	assert(x>0 && x <= s->v_size);
	n = DIVWL(x-1);
	mask = (1 << MODWL(x-1));
	s->v_bits[n] |= mask;
}


Cremove(x,s_p)
	Celem_t x;
	cset    *s_p;
{
	cset s;
	short n;
	int mask;

	s = *s_p;
	assert(x>0 && x <= s->v_size);
	n = DIVWL(x-1);
	mask = (1 << MODWL(x-1));
	s->v_bits[n] &= ~mask;
}



/* The operations first, next and elem can be used to iterate
 * over a set. For example:
 *	for (i = Cfirst(s); i != (Cindex) 0; i = Cnext(i,s) {
 *		x = Celem(i);
 *		use x
 *	}
 * which is like:
 *      'for all elements x of s do'
 *		use x
 *
 * The implementation of first and next is not very fast.
 * It could be made much more efficient (at the price of a
 * higher complexity) by not using 'is_elem'.
 * Iteration over a bitvector, however, is not supposed to
 * be used very often.
 */

Cindex Cfirst(s)
	cset s;
{
	return Cnext((Cindex) 0,s);
}


Cindex Cnext(i,s)
	Cindex i;
	cset   s;
{
	register short n;

	for (n = i+1; n <= s->v_size; n++) {
		if (Cis_elem(n,s)) {
			return (Cindex) n;
		}
	}
	return (Cindex) 0;
}


Celem_t Celem(i)
	Cindex i;
{
	return (Celem_t) i;
}



Cjoin(s1,s2_p)
	cset s1, *s2_p;
{
	/* Two sets are joined by or-ing their bitvectors,
	 * word by word.
	 */

	cset s2;
	short n;
	register short i;

	s2 = *s2_p;
	assert(s1->v_size == s2->v_size);
	n = DIVWL(s1->v_size -1);  /* #words -1 */
	for (i = 0; i <= n; i++) {
		s2->v_bits[i] |= s1->v_bits[i];
	}
}



Cintersect(s1,s2_p)
	cset s1, *s2_p;
{
	/* Two sets are intersected by and-ing their bitvectors,
	 * word by word.
	 */

	cset s2;
	short n;
	register short i;

	s2 = *s2_p;
	assert(s1->v_size == s2->v_size);
	n = DIVWL(s1->v_size -1);  /* #words -1 */
	for (i = 0; i <= n; i++) {
		s2->v_bits[i] &= s1->v_bits[i];
	}
}


Cdeleteset(s)
	cset s;
{
	oldbitvect(s,DIVWL(s->v_size - 1) + 1);
}


bool Cis_subset(s1,s2)
	cset s1,s2;
{
	/* See if s1 is a subset of s2 */

	register short i;

	assert(s1->v_size == s2->v_size);
	if (s1->v_size == 0) return TRUE;
	for (i = 0; i <= DIVWL(s1->v_size-1); i++) {
		if ((s1->v_bits[i] & ~(s2->v_bits[i])) != 0) {
			return FALSE;
		}
	}
	return TRUE;
}


Cclear_set(s_p)
	cset *s_p;
{
	cset s;
	register short i;

	s = *s_p;
	assert (s != (cset) 0);
	for (i = 0; i <=  DIVWL(s->v_size-1); i++) {
		s->v_bits[i] = 0;
	}
}


Ccopy_set(s1,s2_p)
	cset s1, *s2_p;
{
	cset s2;
	register short i;

	s2 = *s2_p;
	assert (s1->v_size == s2->v_size);
	for (i = 0; i <=  DIVWL(s1->v_size-1); i++) {
		s2->v_bits[i] = s1->v_bits[i];
	}
}


Csubtract(s1,s2_p)
	cset s1, *s2_p;
{
	cset s2;
	register short i;

	s2 = *s2_p;
	assert (s1->v_size == s2->v_size);
	for (i = 0; i <=  DIVWL(s1->v_size-1); i++) {
		s2->v_bits[i] &= ~(s1->v_bits[i]);
	}
}


bool Cequal(s1,s2)
	cset s1, s2;
{
	register short i;

	assert (s1->v_size == s2->v_size);
	for (i = 0; i <=  DIVWL(s1->v_size-1); i++) {
		if (s1->v_bits[i] != s2->v_bits[i]) return FALSE;
	}
	return TRUE;
}

short Cnrelems(s)
	cset s;
{
	register short n, cnt;

	cnt = 0;
	for (n = 1; n <= s->v_size; n++) {
		if (Cis_elem(n,s)) {
			cnt++;
		}
	}
	return cnt;
}
