/* $Id$ */
/* library routine for copying structs */

__stb(n, f, t)
	char *f, *t; n;
{
	if (n > 0)
		do
			*t++ = *f++;
		while (--n);
}
