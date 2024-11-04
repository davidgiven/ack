/* $Id$ */
int
strlen(s)
	char *s;
{
	char *b = s;

	while (*b++)
		;
	return b - s - 1;
}
