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
 * global.c
 * Contains declarations visible in several other source files
 */

# include "types.h"
# include "extern.h"
# include "io.h"

# ifndef NORCSID
static string rcsid4 = "$Id$";
# endif

char	ltext[LTEXTSZ];
p_nont	nonterms;
p_nont	maxnt;
int	nnonterms;
p_token	tokens;
p_token	maxt;
int	ntokens;
int	nterms, nalts;
int	norder, torder;
p_start	start;
int	linecount;
int	assval;
FILE	*fout;
FILE	*fpars;
FILE	*finput;
FILE	*fact;
char	f_pars[] = PARSERFILE;
char	f_temp[] = ACTFILE;
char	f_out[20];
string	f_input;
char	f_include[20];
char	f_rec[20];
string	e_noopen = "Cannot open %s";
int	verbose;
int	wflag;
string	lexical;
string	prefix;
string	onerror;
int	ntneeded;
int	ntprint;
# ifndef NDEBUG
int	debug;
#endif /* not NDEBUG */
p_file	files;
p_file	maxfiles;
p_file	pfile;
string	LLgenid = "/* LLgen generated code from source %s */\n";
t_token lextoken;
int	nerrors;
string	rec_file, incl_file;
int	low_percentage = 10, high_percentage = 30;
int	min_cases_for_jmptable = 8;
int	jmptable_option;
int	ansi_c = 0;
int	strip_grammar = 0;
