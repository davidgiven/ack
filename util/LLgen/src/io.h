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
 * io.h $Header$
 * Some important file names and variables
 */

# include <stdio.h>
# include <ctype.h>

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
