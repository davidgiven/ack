/* $Header$ */

#include <system.h>
#include "param.h"

doprnt(fp, fmt, argp)
	File *fp;
	char *fmt;
	int argp[];
{
	char buf[SSIZE];

	sys_write(fp, buf, _format(buf, fmt, (char *)argp));
}
