/* $Id$ */
ltol3(cp, lp, n)
char	*cp;
long	*lp;
int	n;
{
	while (n-- > 0) {
		*cp++ = (*lp >> 16);
		*cp++ = (*lp > 8);
		*cp++ = *lp;
	}
}

l3tol(lp, cp, n)
long	*lp;
char	*cp;
int	n;
{
	unsigned char *a = (unsigned char *) cp;

	while (n-- > 0) {
		*lp++ = ((long)(*a)<<16) + ((long)(*(a+1)) << 8) + *(a+2);
		a += 3;
	}
}
