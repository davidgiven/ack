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
 * $Id$
 * Some important file names and variables
 */

# include <stdio.h>

/* FILES */

# define OUTFILE	"%s.output"	/* -v option */
# define PARSERFILE	"xxxXXXXXX"	/* This is what we want */
# define ACTFILE	"tempXXXXXX"	/* temporary file to save actions */
# define HFILE		"%spars.h"	/* file for "#define's " */
# define RFILE		"%spars.c"	/* Error recovery */
#ifdef NON_CORRECTING
# define NCFILE		"%sncor.c"	/* Non-corrcting error recovery */
#endif
extern FILE *finput;
extern FILE *fpars;
extern FILE *fact;
extern FILE *fout;
extern char	f_pars[];
extern char	f_temp[];
extern char	f_out[];
extern string	f_input;
extern char	f_include[];
extern char	f_rec[];
#ifdef NON_CORRECTING
extern char	f_nc[];
#endif
