/* btszero()
*/

char *
btszero(b, n)
	register char *b;
	register int n;
{
	while (n-- > 0)
		*b++ = '\0';
}
