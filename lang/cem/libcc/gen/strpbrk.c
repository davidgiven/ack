/* $Id$ */
char *
strpbrk(string, brk)
	register char *string, *brk;
{
	register char *s1;

	while (*string) {
		for (s1 = brk; *s1 && *s1 != *string; s1++) /* nothing */ ;
		if (*s1) return string;
		string++;
	}
	return 0;
}
