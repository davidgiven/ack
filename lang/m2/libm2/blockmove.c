/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	block moves
  Author:	Ceriel J.H. Jacobs
  Version:	$Id$
*/
#include <stdint.h>
#include "libm2.h"

void blockmove(size_t siz, char* dst, char* src)
{
	while (siz--)
		*dst++ = *src++;
}
