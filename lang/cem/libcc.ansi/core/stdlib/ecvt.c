/* $Id$ */

#include <math.h>
#include <ack/config.h>

#if ACKCONF_WANT_STDIO_FLOAT

#include "ext_fmt.h"

static char*
cvt(long double value, int ndigit, int* decpt, int* sign, int ecvtflag)
{
	struct EXTEND e;

	_dbl_ext_cvt(value, &e);
	return _ext_str_cvt(&e, ndigit, decpt, sign, ecvtflag);
}

char* _ecvt(long double value, int ndigit, int* decpt, int* sign)
{

	return cvt(value, ndigit, decpt, sign, 1);
}

char* _fcvt(long double value, int ndigit, int* decpt, int* sign)
{
	return cvt(value, ndigit, decpt, sign, 0);
}

#endif
