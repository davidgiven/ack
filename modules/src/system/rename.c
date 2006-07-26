/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include "system.h"

int
sys_rename(path1, path2)
	char *path1, *path2;
{
	return rename(path1, path2) == 0;
}

