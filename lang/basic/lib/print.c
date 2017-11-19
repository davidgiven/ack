#include <stdlib.h>
#include <stdio.h>
#include "bc_string.h"
#include "bc_io.h"

/* $Id$ */

/* Here all routine to generate terminal oriented output is located */

void _qstmark(void)
{
	/* prompt for terminal input */
	putchar('?');
}

void _nl(void)
{
	_asschn();
	_outnl();
}
void _prinum(int i)
{
	char buffer[40];

	_asschn();
	if (i >= 0)
		sprintf(buffer, " %d ", i);
	else
		sprintf(buffer, "-%d ", -i);
	_out(buffer);
}
void _str(double f, char* buffer)
{
	register char* c = buffer;
	int eformat = 0;
	if (f >= 0)
	{
		if (f > 1.0e8)
		{
			eformat = 1;
			sprintf(buffer, " %e", f);
		}
		else
			sprintf(buffer, " %f", f);
		c++;
	}
	else
	{
		if (-f > 1.0e8)
		{
			eformat = 1;
			sprintf(buffer, "-%e", -f);
		}
		else
			sprintf(buffer, "-%f", -f);
	}
	if (!eformat)
	{
		for (; *c && *c != ' '; c++)
			;
		c--;
		while (c > buffer && *c == '0')
		{
			*c = 0;
			c--;
		}
		if (*c == '.')
			*c = 0;
	}
}
void _prfnum(double f)
{
	/* BASIC strings trailing zeroes */
	char buffer[100];
	char* c;

	_asschn();
	c = buffer;
	_str(f, c);
	strcat(buffer, " ");
	_out(buffer);
}
void _prstr(String* str)
{
	_asschn();
	if (str == 0)
		_out("<null>");
	else
		_out(str->strval);
}
