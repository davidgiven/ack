char *
strrindex(str, chr)
	register char *str, chr;
{
	register char *retptr = 0;

	while (*str)
		if (*str++ == chr)
			retptr = &str[-1];
	return retptr;
}
