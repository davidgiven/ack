/*
 * remove.c - remove a file
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#if ACKCONF_WANT_EMULATED_REMOVE

int remove(const char* filename)
{
	return unlink(filename);
}

#endif
