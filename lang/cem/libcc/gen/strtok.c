/* $Id$ */
extern char *strpbrk();

char *
strtok(string, separators)
	register char *string;
	char *separators;
{
	register char *s1, *s2;
	static char *savestring;

	if (!string) string = savestring;

	if (!string) return 0;

	if (*(s1 = string + strspn(string, separators)) == '\0') {
		savestring = 0;
		return 0;
	}

	if (s2 = strpbrk(s1, separators)) {
		*s2++ = '\0';
	}
	savestring = s2;
	return s1;
}
