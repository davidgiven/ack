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
	
extern char *_ext_str_cvt();

static char *
cvt(value, ndigit, decpt, sign, ecvtflag)
	double value;
	int ndigit, *decpt, *sign;
{
	struct EXTEND e;

	_dbl_ext_cvt(value, &e);
	return _ext_str_cvt(&e, ndigit, decpt, sign, ecvtflag);
}

char *
ecvt(value, ndigit, decpt, sign)
	double value;
	int ndigit, *decpt, *sign;
{

	return cvt(value, ndigit, decpt, sign, 1);
}

char *
fcvt(value, ndigit, decpt, sign)
	double value;
	int ndigit, *decpt, *sign;
{
	return cvt(value, ndigit, decpt, sign, 0);
}

#endif /* NOFLOAT */
