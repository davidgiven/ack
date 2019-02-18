/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <sys/stat.h>
#include "system.h"

int sys_chmode(char* path,int mode)
{
	return chmod(path, (mode_t)mode) == 0;
}
