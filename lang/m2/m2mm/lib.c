/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* L I B R A R Y */

/* $Header$ */

#include <em_path.h>

static char lib_dir[128] = EM_DIR;

is_library_dir(d)
	char *d;
{
	/*	Check if directory d is a directory containing
		"system" definition modules. Return 1 if it is, 0 otherwise.
	*/

	return strcmp(lib_dir, d) == 0;
}

init_lib()
{
	extern char *strcat();

	strcat(lib_dir, "/lib/m2");
	AddInclDir(lib_dir);
}
