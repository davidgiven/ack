/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include "system.h"

int
sys_chmode(path, mode)
	char *path;
	int mode;
{
	return chmod(path, mode) == 0;
}
