/*
 * rename.c - rename a file
 */
/* $Header$ */

#include	<stdio.h>

int link(const char *name1, const char *name2);
int unlink(const char *path);

int
rename(const char *old, const char *new) {
	if (!link(old, new))
		return remove(old);
	else return -1;
}
