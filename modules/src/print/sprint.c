/* $Header$ */

#include <system.h>
#include "param.h"

/*VARARGS1*/
char *
sprint(buf, fmt, args)
	char *buf, *fmt;
	int args;
{
	buf[_format(buf, fmt, &args)] = '\0';
	return buf;
}
