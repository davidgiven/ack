/* append t to s, upto n characters	
*/
char *
strncat(s, t, n)
	register char *s, *t;
	register int n;
{
	register char *b = s;

	while (*s++)
		;
	s--;
	while ((n-- > 0) && (*s++ = *t++))
		;
	return b;
}
