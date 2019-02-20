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
#ifdef USE_SYS
#include <system.h>
#endif
#include <stdlib.h>
#include <string.h>
# include "alloc.h"
# include "extern.h"
# include "types.h"

# ifndef NORCSID
static string rcsid5 = "$Id$";
# endif

#ifndef LIBDIR
#define LIBDIR "lib"
#endif

void UNLINK(string x)
{
	/* Must remove the file "x" */
#ifdef USE_SYS
	sys_remove(x);	/* systemcall to remove file */
#else
	remove(x);
#endif
}

void RENAME(string x,string y)
{
	/* Must move the file "x" to the file "y" */

#ifdef USE_SYS
	if(!sys_rename(x,y)) fatal(1,"Cannot rename to %s",y);
#else
	if (rename(x, y) == -1)
		fatal(1, "Cannot rename to %s", y);
#endif
}

string libpath(string s)
{
	/* Must deliver a full pathname to the library file "s" */
	register string p;
	register int length;

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

void TMPNAM(string result)
{
   if (tmpnam(result)==NULL)
   {
	   fatal(1, "Cannot create temporary file.", NULL);
   }
}
