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
 * io.h $Header$
 * Some important file names and variables
 */

# include <stdio.h>

/* FILES */

# define OUTFILE	"%s.output"	/* -v option */
# define PARSERFILE	"xxxXXXXXX"	/* This is what we want */
# define ACTFILE	"tempXXXXXX"	/* temporary file to save actions */
# define HFILE		"%spars.h"	/* file for "#define's " */
# define RFILE		"%spars.c"	/* Error recovery */

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
