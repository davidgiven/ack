/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	bts2str() turns a row of bytes b with length n into string s
	The ASCII set of characters is used.
	86/03/17 EHB
*/

#define is_print(c)	((unsigned)((c) - ' ') <= '~' - ' ')

char *
bts2str(b, n, s)
	char *b, *s;
	register int n;
{
	register char *f = b, *t = s;

	while (n-- > 0) {
		if (is_print(*f))
			*t++ = *f++;
		else {
			register char *p;
			register int n = (*f++ & 0377);

			*t = '\\';
			p = (t += 4);
			*--p = (n & 07) + '0';
			n >>= 3;
			*--p = (n & 07) + '0';
			n >>= 3;
			*--p = (n & 07) + '0';
			n >>= 3;
		}
	}
	*t = '\000';
	return s;
}
