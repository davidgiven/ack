/* $Id$ */
int
bcmp(b1, b2, n)
	register char *b1, *b2;
	register int n;
{
	register int i;

	while (n--) {
		if (i = *b2++ - *b1++) return i;
	}
	return 0;
}
