/*
 * rename.c - rename a file
 */
/* $Id$ */

#if !defined(_POSIX_SOURCE)
#include	<stdio.h>

int _link(const char *name1, const char *name2);

int
rename(const char *old, const char *new) {
	if (!_link(old, new))
		return remove(old);
	else return -1;
}
#endif
