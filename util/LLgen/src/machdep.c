/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
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


# include <em_path.h>
# include "types.h"

# ifndef NORCSID
static string rcsid5 = "$Header$";
# endif

/* In this file the following routines are defined: */
extern	UNLINK();
extern	RENAME();
extern string	libpath();

UNLINK(x) string x; {
	/* Must remove the file "x" */

	sys_remove(x);	/* systemcall to remove file */
}

RENAME(x,y) string x,y; {
	/* Must move the file "x" to the file "y" */

	if(! sys_rename(x,y)) fatal(1,"Cannot rename to %s",y);
}

/* to make it easier to patch ... */
char emdir[64] = EM_DIR;

string
libpath(s) string s; {
	/* Must deliver a full pathname to the library file "s" */

	register string p;
	register length;
	p_mem alloc();
	string strcpy(), strcat();
	static string subdir = "/lib/LLgen/";

	length = strlen(emdir) + strlen(subdir) + strlen(s) + 1;
	p = (string) alloc((unsigned) length);
	strcpy(p,emdir);
	strcat(p,subdir);
	strcat(p,s);
	return p;
}
