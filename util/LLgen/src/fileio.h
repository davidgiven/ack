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

#include <stdio.h>
#include "types.h"

/* FILES */

#define OUTFILE "%s/%s.output" /* -v option */
#define HFILE "%s/%spars.h" /* file for "#define's " */
#define RFILE "%s/%spars.c" /* Error recovery */
#ifdef NON_CORRECTING
#define NCFILE "%s/%sncor.c" /* Non-corrcting error recovery */
#endif
extern FILE* finput;
extern FILE* fpars;
extern FILE* fact;
extern FILE* fout;
extern string f_dir;
extern string f_pars;
extern string f_temp;
extern string f_out;
extern string f_input;
extern string f_include;
extern string f_rec;
#ifdef NON_CORRECTING
extern string f_nc;
#endif
