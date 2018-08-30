/* $Id$ */

#include <stdlib.h>
#include <ack/config.h>
#include "ext_fmt.h"

#if ACKCONF_WANT_FLOAT

double
strtod(const char* p, char** pp)
{
	struct EXTEND e;

	_str_ext_cvt(p, pp, &e);
	return _ext_dbl_cvt(&e);
}

#endif
