/* $Header$ */
#include <stdio.h>

int fscanf (fp, format, args)
FILE           *fp;
char           *format;
unsigned        args;
{
	return _doscanf (fp, format, &args);
}

