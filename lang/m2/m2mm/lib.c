/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* L I B R A R Y */

/* $Id$ */

#include <em_path.h>
#include <alloc.h>
#include "main.h"

#ifdef OTHER_HOME
#undef EM_DIR
#define EM_DIR OTHER_HOME
#endif
static char lib_dir[128] = EM_DIR;

static struct liblist {
	int libno;
	struct liblist *libnext;
} *lblist;

int
is_library_dir(d)
	char *d;
{
	/*	Check if directory d is a directory containing
		"system" definition modules. Return 1 if it is, 0 otherwise.
	*/

	register struct liblist *p = lblist;

	while (p) {
		if (! strcmp(DEFPATH[p->libno], d)) return 1;
		p = p->libnext;
	}
	return 0;
}

#ifndef DEF_DIR
#define DEF_DIR	"lib/m2"
#endif

init_lib()
{
	extern char *strcat();

	strcat(lib_dir, "/");
	strcat(lib_dir, DEF_DIR);
	AddLibDir(lib_dir);
}

set_libdir(n)
{
	register struct liblist *p = 
		(struct liblist *) Malloc(sizeof(struct liblist));

	p->libnext = lblist;
	p->libno = n;
	lblist = p;
}
