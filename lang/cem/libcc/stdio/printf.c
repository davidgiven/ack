/* $Header$ */
#include <stdio.h>

printf (fmt, args)
char *fmt;
int args;
{
	_doprnt (fmt, &args, stdout);
	if ( io_testflag(stdout,IO_PERPRINTF) )
        	fflush(stdout);
}
