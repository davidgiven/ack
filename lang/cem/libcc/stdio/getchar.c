/* $Id$ */
#include <stdio.h>

#undef getchar

getchar()
{
	return getc(stdin);
}
