/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

/* Author: M.L. Kersten
** Here all the global objects are defined.
*/
#include "symbols.h"
#include "graph.h"
#include "y.tab.h"

#ifndef NORCSID
# define RCS_BEM	"$Header$"
#endif

#define MAXINT		32768
#define MININT		-32767
#define EMINTSIZE	"EM_WSIZE"
#define EMPTRSIZE	"EM_PSIZE"
#define EMFLTSIZE	"EM_DSIZE"

#define MAXPIECES	100
#define MAXFILENAME	200

#define CHANNEL		0
#define THRESHOLD	40		/* for splitting blocks */

#define void		int		/* Some C compilers don't know void */

extern char	*program;		/* name of source program */
extern char	*inpfile;		/* input tko compiler */
extern char	*outfile;		/* output from compiler */

extern char	datfname[MAXFILENAME];	/* data statements file */
extern char	tmpfname[MAXFILENAME];	/* temporary statements file */

extern FILE	*emfile;		/* EM output file */
extern FILE	*datfile;		/* data file */
extern FILE	*Tmpfile;		/* compiler temporary */
extern FILE	*yyin;			/* Compiler input */

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

extern int	emlinecount;		/* counts lines on Tmpfile */
extern int 	dataused;
extern int	typetable[10];		/* parameters to standard functions */

extern Linerecord *currline;


extern char *itoa();
extern char *datalabel();
extern char *instrlabel();
extern char *proclabel();
extern char *typesize();
extern char *typestring();
extern char *salloc();

extern char *sprintf();
extern char *strcpy();
extern char *strcat();
extern char *malloc();
