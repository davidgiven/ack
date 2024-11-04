/* $Id$ */
char *
strpbrk(string, brk)
	char *string, *brk;
{
	char *s1;

	while (*string) {
		for (s1 = brk; *s1 && *s1 != *string; s1++) /* nothing */ ;
		if (*s1) return string;
		string++;
	}
	return 0;
}
