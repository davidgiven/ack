/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 *
 * Module:	CARDINAL operations with overflow checking
 * Author:	Ceriel J.H. Jacobs
 * Version:	$Id$
*/

#ifndef EM_WSIZE
#define EM_WSIZE _EM_WSIZE
#endif
#ifndef EM_LSIZE
#define EM_LSIZE _EM_LSIZE
#endif

#include "libm2.h"
#include <m2_traps.h>

#define MAXCARD ((unsigned)-1)
#if EM_WSIZE < EM_LSIZE
#define MAXLONGCARD ((unsigned long)-1L)
#endif

void adduchk(unsigned int a, unsigned int b)
{
	if (MAXCARD - a < b)
		TRP(M2_UOVFL);
}

#if EM_WSIZE < EM_LSIZE
void addulchk(unsigned long a, unsigned long b)
{
	if (MAXLONGCARD - a < b)
		TRP(M2_UOVFL);
}
#endif

void muluchk(unsigned int a, unsigned int b)
{
	if (a != 0 && MAXCARD / a < b)
		TRP(M2_UOVFL);
}

#if EM_WSIZE < EM_LSIZE
void mululchk(unsigned long a, unsigned long b)
{
	if (a != 0 && MAXLONGCARD / a < b)
		TRP(M2_UOVFL);
}
#endif

void subuchk(unsigned int a, unsigned int b)
{
	if (b < a)
		TRP(M2_UUVFL);
}

#if EM_WSIZE < EM_LSIZE
void subulchk(unsigned long a, unsigned long b)
{
	if (b < a)
		TRP(M2_UUVFL);
}
#endif
