/* $Header$ */

#include <system.h>
#include "param.h"

/*VARARGS1*/
print(fmt, args)
	char *fmt;
	int args;
{
	char buf[SSIZE];

	sys_write(STDOUT, buf, _format(buf, fmt, &args));
}
