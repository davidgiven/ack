/* $Id$ */
#ifndef NOFLOAT

struct mantissa {
	unsigned long h_32;
	unsigned long l_32;
};

struct EXTEND {
	short	sign;
	short	exp;
	struct mantissa mantissa;
#define m1 mantissa.h_32
#define m2 mantissa.l_32
};
	

extern double _ext_dbl_cvt();

double
strtod(p, pp)
	char *p, **pp;
{
	struct EXTEND e;

	_str_ext_cvt(p, pp, &e);
	return _ext_dbl_cvt(&e);
}
#endif /* NOFLOAT */
