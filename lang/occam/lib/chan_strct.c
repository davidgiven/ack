/* $Id$ */
/*	chan_struct.c - channel routines for more structured objects */
#include "ocm_chan.h"

void cbyte_in(b, c) char *b; chan *c;
{
	long v;
	chan_in(&v, c);
	*b= (char) v;
}

void c_wa_in(a, z, c) register long *a; register unsigned z; register chan *c;
{
	do
		chan_in(a++, c);
	while (--z!=0);
}

void c_ba_in(a, z, c) register char *a; register unsigned z; register chan *c;
{
	do {
		long v;
		chan_in(&v, c);
		*a++ = (char) v;
	} while (--z!=0);
}

void c_wa_out(a, z, c) register long *a; register unsigned z; register chan *c;
{
	do
		chan_out(*a++, c);
	while (--z!=0);
}

void c_ba_out(a, z, c) register char *a; register unsigned z; register chan *c;
{
	do
		chan_out((long) (*a++ &0377), c);
	while (--z!=0);
}
