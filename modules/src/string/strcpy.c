/* Copy t into s
*/
char *
strcpy(s, t)
	register char *s, *t;
{
	register char *b = s;

	while (*s++ = *t++)
		;
	return b;
}
