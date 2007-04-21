/*
 * remove.c - remove a file
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int
remove(const char *filename) {
	return unlink(filename);
}
