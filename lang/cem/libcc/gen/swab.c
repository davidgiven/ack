/* $Id$ */
swab(from, to, nbytes)
	register char *from, *to;
{
	nbytes /= 2;
	while (nbytes-- > 0) {
		*(to+1) = *from++;
		*to = *from++;
		to += 2;
	}
}
