/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
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


# include "../../../h/em_path.h"
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

	unlink(x);	/* systemcall to remove file */
}

RENAME(x,y) string x,y; {
	/* Must move the file "x" to the file "y" */

	unlink(y);
	if(link(x,y)!=0)fatal(1,"Cannot link to %s",y);
	unlink(x);
}

string
libpath(s) string s; {
	/* Must deliver a full pathname to the library file "s" */

	static char buf[100];
	string strcpy(), strcat();

	strcpy(buf,EM_DIR);
	strcat(buf,"/lib/LLgen/");
	strcat(buf,s);
	return buf;
}
