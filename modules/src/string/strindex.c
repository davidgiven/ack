/* strindex() -- (86/03/18 EHB)
*/

char *
strindex(s, c)
	register char *s, c;
{
	while (*s)
		if (*s++ == c)
			return --s;
	return (char *)0;
}
