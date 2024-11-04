/* $Id$ */
int
strcmp(s, t)
	char *s, *t;
{
	while (*s == *t++)
		if (*s++ == '\0')
			return 0;
	return *s - *--t;
}
