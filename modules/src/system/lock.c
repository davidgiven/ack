/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include "system.h"

int
sys_lock(path)
	char *path;
{
	char buf[1024];
	char *tmpf = ".lockXXXXXX";
	char *strrindex(), *strcpy(), *mktemp();
	char *p;
	int ok, fd;

	strcpy(buf, path);
	if (p = strrindex(buf, '/')) {
		++p;
		strcpy(p, tmpf);
	}
	else
		strcpy(buf, tmpf);
	mktemp(buf);
	if ((fd = creat(buf, 0)) < 0)
		return 0;
	close(fd);
	ok = (link(buf, path) == 0);
	unlink(buf);
	return ok;
}
