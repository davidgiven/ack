/* $Header$ */
#include <stdio.h>

fprintf (file, fmt, args)
FILE *file;
char *fmt;
int args;
{
	_doprnt (fmt, &args, file);
	if ( io_testflag(file,IO_PERPRINTF) )
        	fflush(file);
}
