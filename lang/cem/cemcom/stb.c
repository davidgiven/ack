/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* library routine for copying structs */

__stb(n, f, t)
	register char *f, *t; register n;
{
	if (n > 0)
		do
			*t++ = *f++;
		while (--n);
}
