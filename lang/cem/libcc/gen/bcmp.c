/* $Id$ */
int
bcmp(b1, b2, n)
	char *b1, *b2;
	int n;
{
	int i;

	while (n--) {
		if (i = *b2++ - *b1++) return i;
	}
	return 0;
}
