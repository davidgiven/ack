/* $Id$ */
/* find first occurrence of wanted in s */
char *
strstr(s, wanted)
	register char *s, *wanted;
{
	int len = strlen(wanted);

	while (*s != *wanted || strncmp(s, wanted, len)) {
		if (*s++ == '\0') return 0;
	}
	return s;
}
