/* str2long()
*/

value(c, b)
	char c;
	int b;
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return b;
}

long
str2long(str, base)
	register char *str;
	int base;
{
	int minus = 0, d;
	long l = 0;

	if (*str == '-') {
		minus++;
		str++;
	}
	while ((d = value(*str++, base)) < base)
		l = base * l + d;
	return minus ? -l : l;
}
