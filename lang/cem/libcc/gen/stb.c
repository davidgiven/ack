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
