/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * For full copyright and restrictions on use see the file COPYING in the top
 * level of the LLgen tree.
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * machdep.c
 * Machine dependant things
 */
#if defined _WIN32
#include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
# include "extern.h"
# include "types.h"

# ifndef NORCSID
static string rcsid5 = "$Id$";
# endif

#ifndef LIBDIR
#define LIBDIR "lib"
#endif


string libpath(string s)
{
	/* Must deliver a full pathname to the library file "s" */
	register string p;
	register size_t length;

	char* libdir = getenv("LLGEN_LIB_DIR");
	if (!libdir)
		libdir = LIBDIR;
	return aprintf("%s/%s", libdir, s);
}

string maketempfile()
{
	string tmp = getenv("TMP");
	if (!tmp)
		tmp = "/tmp";

	string filename = aprintf("%s/llgen-XXXXXX", tmp);
	int fd = mkstemp(filename);
	if (fd == -1)
	   fatal(1, "Cannot create temporary file.", NULL);

	close(fd);
	return filename;
}
