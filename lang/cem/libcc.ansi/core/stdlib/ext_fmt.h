#ifndef _EXT_FMT_H
#define _EXT_FMT_H

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
	
extern void _str_ext_cvt(const char* s, char** ss, struct EXTEND* e);
extern double _ext_dbl_cvt(struct EXTEND* e);
extern void _dbl_ext_cvt(double value, struct EXTEND* e);
extern char* _ext_str_cvt(struct EXTEND* e, int ndigit, int* decpt, int* sign, int ecvtflag);

#endif
