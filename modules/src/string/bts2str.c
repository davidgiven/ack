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
			register int n = (*f++ & 0377);
			register int i;
			register char *p;

			*t = '\\';
			p = (t += 4);
			for (i = 0; i < 3; i++) {
				*--p = (n & 07) + '0';
				n >>= 3;
			}
		}
	}
	*t = '\000';
	return s;
}
