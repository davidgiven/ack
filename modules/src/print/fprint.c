/* $Header$ */

#include <system.h>
#include "param.h"

/*VARARGS1*/
fprint(fp, fmt, args)
	File *fp;
	char *fmt;
	int args;
{
	char buf[SSIZE];

	sys_write(fp, buf, _format(buf, fmt, &args));
}
