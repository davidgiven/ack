/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
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
