/*	return negative, zero or positive value if
	resp. s < t, s == t or s > t
*/
int
strcmp(s, t)
	register char *s, *t;
{
	while (*s == *t++)
		if (*s++ == '\0')
			return 0;
	return *s - *--t;
}
