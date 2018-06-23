/*
 * rename.c - rename a file
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* Disabled, dtrg: rename is a system call these days. */
#if 0
int _link(const char *name1, const char *name2);

int
rename(const char *old, const char *new) {
	if (!_link(old, new))
		return remove(old);
	else return -1;
}
#endif
