/* append t to s
*/
char *
strcat(s, t)
	register char *s, *t;
{
	register char *b = s;

	while (*s++)
		;
	s--;
	while (*s++ = *t++)
		;
	return b;
}
