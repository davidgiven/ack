/* $Id$ */
#include <stdio.h>

#undef putchar

putchar(c)
{
	return putc(c, stdout);
}
