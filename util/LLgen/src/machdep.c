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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "extern.h"

# ifndef NORCSID
static string rcsid5 = "$Id$";
# endif

#define LIBDIR "lib"

void UNLINK(string x)
{
	/* Must remove the file "x" */
	remove(x);
}

void RENAME(string x,string y)
{
	/* Must move the file "x" to the file "y" */
	if (rename(x, y) == -1)
		fatal(1, "Cannot rename to %s", y);
}

string libpath(string s)
{
	/* Must deliver a full pathname to the library file "s" */

	register string p;
	register int length;
	p_mem alloc();

	char* libdir = getenv("LLGEN_LIB_DIR");
	if (!libdir)
		libdir = LIBDIR;
	length = strlen(libdir) + strlen(s) + 2;
	p = (string) alloc((unsigned) length);
	strcpy(p,libdir);
	strcat(p,"/");
	strcat(p,s);
	return p;
}
