/* $Id$ */
bcopy(old, new, n)
char *old, *new;
int n;
{
/* Copy a block of data. */

	if (old <= new && old + (n-1) >= new) {
		old += n; new += n;
		while (n-- > 0) *--new = *--old;
	}
	else	while (n-- > 0) *new++ = *old++;
}
