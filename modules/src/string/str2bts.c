/* str2bts -- (1985, EHB)
*/
static
is_oct(c)
	char c;
{
	return (c == '0' || c == '1' || c == '2' || c == '3' ||
		c == '4' || c == '5' || c == '6' || c == '7');
}

/*	str2bts() strips the escaped characters of a
	string and replaces them by the ascii characters they stand for.
	The ascii length of the resulting string is returned, including the
	terminating null-character.
*/
char *
str2bts(str, bts, pn)
	register char *str;
	char *bts;
	int *pn;
{
	register char *t = bts;

	while (*str) {
		if (*str == '\\') {
			switch (*++str) {
			case 'b':
				*t++ = '\b';
				str++;
				break;
			case 'f':
				*t++ = '\f';
				str++;
				break;
			case 'n':
				*t++ = '\n';
				str++;
				break;
			case 'r':
				*t++ = '\r';
				str++;
				break;
			case 't':
				*t++ = '\t';
				str++;
				break;
			default:
				if (is_oct(*str)) {
					register cnt = 0, oct = 0;

					do
						oct = oct * 8 + *str - '0';
					while (is_oct(*++str) && ++cnt < 3);
					*t++ = (char) oct;
					break;
				}
				*t++ = *str++;
				break;
			}
		}
		else
			*t++ = *str++;
	}
	*t = '\0';	/* don't forget this one !!!	*/
	*pn = t - bts + 1;
	return bts;
}
