/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <ctype.h>
#include <signal.h>
#include <system.h>
#include <print.h>
#include <em.h>
#include <em_mes.h>

/* Author: M.L. Kersten
** Here all the global objects are defined.
*/
#include "symbols.h"
#include "graph.h"
#include "Lpars.h"

#ifndef NORCSID
# define RCS_BEM	"$Id$"
#endif

#define MAXINT		32768
#define MININT		-32767
/* #define EMINTSIZE	"EM_WSIZE" */
/* #define EMPTRSIZE	"EM_PSIZE" */
/* #define EMFLTSIZE	"EM_DSIZE" */

#define EMINTSIZE 4
#define EMPTRSIZE 4
#define EMFLTSIZE 8

#define MAXPIECES	100
#define MAXFILENAME	200

#define CHANNEL		0
#define THRESHOLD	40		/* for splitting blocks */

#ifndef __STDC__
#define void		int		/* Some C compilers don't know void */
#endif

extern int	BEMINTSIZE, BEMPTRSIZE, BEMFLTSIZE;
extern char	*program;		/* name of source program */
extern char	*inpfile;		/* input tko compiler */
extern char	*outfile;		/* output from compiler */

extern char	datfname[MAXFILENAME];	/* data statements file */

extern File	*emfile;		/* EM output file */
extern File	*datfile;		/* data file */
extern File	*yyin;			/* Compiler input */

extern int	endofinput;
extern int 	wflag;
extern int	hflag;
extern int	traceflag;
extern int	yydebug;
extern int	yylineno;
extern int	listing;
extern int	nolins;
extern int	threshold;
extern int	debug;
extern int 	tronoff;
extern label	err_goto_label;

extern int 	dataused;

extern Linerecord *currline;


extern char *itoa();
extern char *salloc();

extern char *strcpy();
extern char *strcat();
#if __STDC__
#include <stdlib.h>
#else
extern char *malloc();
#endif
