/* $Id$ */
bcopy(old, new, n)
register char *old, *new;
register int n;
{
/* Copy a block of data. */

	if (old <= new && old + (n-1) >= new) {
		old += n; new += n;
		while (n-- > 0) *--new = *--old;
	}
	else	while (n-- > 0) *new++ = *old++;
}
