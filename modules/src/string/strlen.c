/* return length of s
*/
int
strlen(s)
	char *s;
{
	register char *b = s;

	while (*b++)
		;
	return b - s - 1;
}
