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

# define OUTFILE	"LL.output"	/* -v option */
# define PARSERFILE	"LL.xxx"	/* This is what we want */
# define ACTFILE	"LL.temp"	/* temporary file to save actions */
# define HFILE		"Lpars.h"	/* file for "#define's " */
# define RFILE		"Lpars.c"	/* Error recovery */

extern FILE *finput;
extern FILE *fpars;
extern FILE *fact;
extern FILE *fout;
extern string	f_pars;
extern string	f_temp;
extern string	f_out;
extern string	f_input;
