#include <stdio.h>

int scanf (format, args)
char           *format;
unsigned        args;
{
	return _doscanf (stdin, format, &args);
}


