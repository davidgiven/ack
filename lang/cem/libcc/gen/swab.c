/* $Id$ */
swab(from, to, nbytes)
	char *from, *to;
{
	nbytes /= 2;
	while (nbytes-- > 0) {
		*(to+1) = *from++;
		*to = *from++;
		to += 2;
	}
}
