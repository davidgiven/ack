#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include <stdio.h>
#include "param.h"
#include "types.h"
#include "lookup.h"
#include "proinf.h"

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

unsigned linecount = 0;		/* "line"number for errormessages */
int	prodepth  = 0;		/* Level of nesting */
bool	Lflag	  = 0;		/* make library module */
bool	nflag	  = 0;		/* do not optimize */
line_p	instrs,pseudos;		/* pointers to chains */
sym_p	symhash[NSYMHASH];	/* array of pointers to chains */
FILE	*outfile;
char	template[] = "/usr/tmp/emoptXXXXXX";
offset	wordsize = 0;
offset	pointersize = 0;
char	*progname;
proinf	curpro;			/* collected information about current pro */
