/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* str2long()
*/

#include "ack_string.h"

value(c, b)
	char c;
	int b;
{
	register int ch;

	ch = c - '0';
	if ((unsigned) ch <= 9) return ch;
	ch = c - 'A';
	if ((unsigned) ch <= 5) return ch + 10;
	ch = c - 'a';
	if ((unsigned) ch <= 5) return ch + 10;
	return b;
}

long
str2long(str, base)
	register char *str;
	int base;
{
	int minus = 0, d;
	long l = 0;

	if (*str == '-') {
		minus++;
		str++;
	}
	while ((d = value(*str++, base)) < base)
		l = base * l + d;
	return minus ? -l : l;
}
