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
 * global.c
 * Contains declarations visible in several other source files
 */

# include "types.h"
# include "io.h"
# include "tunable.h"

static string rcsid = "$Header$";

char	ltext[LTEXTSZ];
t_entry	h_entry[NTERMINALS+NNONTERMS+1];
p_entry	max_t_ent;
t_nont	nonterms[NNONTERMS+1];
int	nnonterms;
int	nterminals;
int	order[NNONTERMS+1];
int	*maxorder;
p_start	start;
int	linecount;
int	assval;
string	pentry[ENTSIZ];
FILE	*fout;
FILE	*fpars;
FILE	*finput;
FILE	*fact;
p_nont	maxnt;
string	f_pars = PARSERFILE;
string	f_out = OUTFILE;
string	f_temp = ACTFILE;
string	f_input;
string	e_noopen = "Cannot open %s";
int	verbose;
string	lexical;
int	ntneeded;
int	ntprint;
# ifndef NDEBUG
int	debug;
# endif not NDEBUG
p_file	files;
p_file	pfile;
string	LLgenid = "/* LLgen generated code from source %s */\n";
t_token lextoken;
int	nerrors;
int	fflag;
