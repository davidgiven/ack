/* btscpy()
*/

char *
btscpy(b1, b2, n)
	register char *b1, *b2;
	register int n;
{
	char *b1s = b1;

	while (n-- > 0)
		*b1++ = *b2++;
	return b1s;
}
