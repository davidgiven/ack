/* Copy t into s, upto n characters
*/
char *
strncpy(s, t, n)
	register char *s, *t;
	register int n;
{
	register char *b = s;

	while ((n-- > 0) && (*s++ = *t++))
		;
	return b;
}
