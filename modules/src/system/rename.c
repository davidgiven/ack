/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include <system.h>

int
sys_rename(path1, path2)
	char *path1, *path2;
{
	unlink(path2);
	return	link(path1, path2) == 0 &&
		unlink(path1) == 0;
}

