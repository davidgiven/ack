#define CODE_EXPANDER
#include <system.h>
#include <out.h>
#include "mach.h"
#include "back.h"

#ifdef DEBUG
arg_error( s, arg)
char *s;
int arg;
{
	fprint( STDERR, "arg_error %s %d\n", s, arg);
}
#endif

con_float(str, argval)
	char *str;
	int argval;
{
	double f;
	double atof();
	int i;
	int j;
	double frexp();
	float fl;
	char *p;

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
	if (argval == 4) {
		/* careful: avoid overflow */
		fl = frexp(f, &i);
		fl = frexp(fl,&j);
		if (i+j > 127) {
			/* overflow situation */
			gen1(0377);
			gen1(f<0?0377:0177);
			gen1(0377);
			gen1(0377);
			return;
		}
		if (i+j < -127) {
			/* underflow situation */
			gen1(0200);
			gen1(f<0?0200:0);
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
}

__as_indexed(val, num)
{
	if (fit_byte(val)) {
		text1( 0xa0 | num);
		text1( val);
	} else if (fit_word(val)) {
		text1( 0xc0 | num);
		text2( val);
	} else {
		text1( 0xe0 | num);
		text4( val);
	}
}

__as_const(val)
{
	if (fit_6bits(val)) {
		text1(val);
	}
	else {
		text1(0x8f);
		text4(val);
	}
}
