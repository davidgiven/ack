#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "param.h"
#include "tables.h"
#include "types.h"
#include "glosym.h"

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
 * Author: Hans van Staveren
 */

extern string myalloc();

glosym_p glolist= (glosym_p) 0;

enterglo(name,romp) string name; word *romp; {
	register glosym_p gp;
	register i;

	gp = (glosym_p) myalloc(sizeof *gp);
	gp->gl_next = glolist;
	gp->gl_name = (string) myalloc(strlen(name)+1);
	strcpy(gp->gl_name,name);
	for (i=0;i<=MAXROM;i++)
		gp->gl_rom[i] = romp[i];
	glolist = gp;
}

glosym_p lookglo(name) string name; {
	register glosym_p gp;

	for (gp=glolist;gp != (glosym_p) 0; gp=gp->gl_next)
		if (strcmp(gp->gl_name,name)==0)
			return(gp);
	return((glosym_p) 0);
}
