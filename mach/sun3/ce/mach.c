#include "mach.h"
#include <back.h>
#include <out.h>
#include <system.h>


arg_error( s, arg)
char *s;
int arg;
{
	fprint( STDERR, "arg_error %s %d\n", s, arg);
}


/*
do_open( filename)
char *filename;
{
	if ( filename == (char *)0 || !sys_open( filename, OP_WRITE, &codefile))
		return( 0);

	fprint( codefile, ".sect .text; .sect .rom; .sect .data; .sect .bss\n");
	return( 1);
}
*/

#define OWNFLOAT	/* compile on SUN, generate code for SUN FP ... */

con_float(str, argval)
	char *str;
	int argval;
{
#ifdef NOFLOAT

static int been_here;
	if (argval != 4 && argval != 8)
		arg_error("fcon", argval);
	if (argval == 8)
		gen4((FOUR_BYTES) 0);
	gen4((FOUR_BYTES) 0);
	if ( !been_here++)
	{
	fputs("Warning : dummy float-constant(s)\n", stderr);
	}
#else
#define IEEEFLOAT
	double f;
	double atof();
	int i;
	int j;
	double frexp();
#ifndef OWNFLOAT
	int sign = 0;
	int fraction[4] ;
#else OWNFLOAT
	float fl;
	char *p;
#endif OWNFLOAT

	if (argval!= 4 && argval!= 8)	{
		arg_error("fcon", argval);
		argval = 8;
	}
	f = atof(str);
	if (f == 0) {
		if (argval == 8) gen4((FOUR_BYTES) 0);
		gen4((FOUR_BYTES) 0);
		return;
	}
#ifdef OWNFLOAT
	if (argval == 4) {
		/* careful: avoid overflow */
		double f1;
		f1 = frexp(f, &i);
		fl = frexp(f1,&j);
		if (i+j > 127) {
			/* overflow situation */
			gen1(f<0?0377:0177);
			gen1(0377);
			gen1(0377);
			gen1(0377);
			return;
		}
		if (i+j < -127) {
			/* underflow situation */
			gen1(f<0?0200:0);
			gen1(0200);
			gen1(0);
			gen1(0);
			return;
		}
		fl = f;
		p = (char *) &fl;
	}
	else {
		p = (char *) &f;
	}
	gen1(*p++&0377);
	for (i = argval-1; i; i--) {
		gen1(*p++&0377);
	}
#else OWNFLOAT
	f = frexp(f, &i);
	if (f < 0) {
		f = -f;
		sign = 1;
	}
	while (f < 0.5) {
		f += f;
		i --;
	}
	f = 2*f - 1.0;		/* hidden bit */
	i--;			/* exponent is one lower for SUN floating point! */
#ifdef IEEEFLOAT
	if (argval == 4) {
#endif IEEEFLOAT
		i = (i + 128) & 0377;
		fraction[0] = (sign << 15) | (i << 7);
		for (j = 6; j>= 0; j--) {
			f *= 2;
			if (f >= 1.0) {
				f -= 1.0;
				fraction[0] |= (1 << j);
			}
		}
#ifdef IEEEFLOAT
	}
	else {
		i = (i + 1024) & 03777;
		fraction[0] = (sign << 15) | (i << 4);
		for (j = 3; j>= 0; j--) {
			f *= 2;
			if (f >= 1.0) {
				fraction[0] |= (1 << j);
				f -= 1.0;
			}
		}
	}
#endif IEEEFLOAT
	for (i = 1; i < argval / 2; i++) {
		fraction[i] = 0;
		for (j = 15; j>= 0; j--) {
			f *= 2;
			if (f >= 1.0) {
				fraction[i] |= (1 << j);
				f -= 1.0;
			}
		}
	}
	if (f >= 0.5) {
		for (i = argval/2 - 1; i >= 0; i--) {
			for (j = 0; j < 16; j++) {
				if (fraction[i] & (1 << j)) {
					fraction[i] &= ~(1 << j);
				}
				else {
					fraction[i] |= (1 << j);
					break;
				}
			}
			if (j != 16) break;
		}
	}
	for (i = 0; i < argval/2; i++) {
		gen1((fraction[i]>>8)&0377);
		gen1(fraction[i]&0377);
	}
#endif OWNFLOAT
#endif
}

#define CODE_EXPANDER
#include <em.h>

#if EM_WSIZE == 1
#define conEM_WSIZE	con1
#define romEM_WSIZE	rom1
#define CAST_WSIZE	ONE_BYTE
#endif
#if EM_WSIZE == 2
#define conEM_WSIZE	con2
#define romEM_WSIZE	rom2
#define CAST_WSIZE	TWO_BYTES
#endif
#if EM_WSIZE == 4
#define conEM_WSIZE	con4
#define romEM_WSIZE	rom4
#define CAST_WSIZE	FOUR_BYTES
#endif

#if EM_PSIZE == 1
#define relocEM_PSIZE	reloc1
#endif
#if EM_PSIZE == 2
#define relocEM_PSIZE	reloc2
#endif
#if EM_PSIZE == 4
#define relocEM_PSIZE	reloc4
#endif
