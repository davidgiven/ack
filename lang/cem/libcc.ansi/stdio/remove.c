/*
 * remove.c - remove a file
 */
/* $Header$ */

#include	<stdio.h>

int unlink(const char *path);

int
remove(const char *filename) {
	return unlink(filename);
}
