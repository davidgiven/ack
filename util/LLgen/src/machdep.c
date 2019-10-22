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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	length = strlen(libdir) + strlen(s) + 2;
	p = (string) alloc(length);
	strcpy(p,libdir);
	strcat(p,"/");
	strcat(p,s);
	return p;
}

void TMPNAM(string result)
{
   if (tmpnam(result)==NULL)
   {
	   fatal(1, "Cannot create temporary file.", NULL);
   }
}
