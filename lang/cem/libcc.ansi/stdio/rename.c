/*
 * rename.c - rename a file
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int _link(const char *name1, const char *name2);

int
rename(const char *old, const char *new) {
	if (!_link(old, new))
		return remove(old);
	else return -1;
}
#endif
