/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include <stdio.h>
#include "param.h"
#include "types.h"
#include "tes.h"
#include "lookup.h"
#include "proinf.h"



unsigned linecount = 0;		/* "line"number for errormessages */
int	prodepth  = 0;		/* Level of nesting */
bool	Lflag	  = 0;		/* make library module */
bool	nflag	  = 0;		/* do not optimize */
int	repl_muls = 0;		/* max # of shifts/adds for replacing muls */
bool	repl_longmuls = 0;	/* replacing longmuls as well? */
line_p	instrs,pseudos;		/* pointers to chains */
sym_p	symhash[NSYMHASH];	/* array of pointers to chains */
FILE	*outfile;
char	*tempname;
offset	wordsize = 0;
offset	pointersize = 0;
char	*progname;
proinf	curpro;			/* collected information about current pro */
