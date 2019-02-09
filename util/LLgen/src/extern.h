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
 * Miscellanious constants and
 * some variables that are visible in more than one file
 */

# define LTEXTSZ	256	/* Size of longest token */

/*
 * options for the identifier search routine
 */

# define ENTERING	1
# define BOTH		2

/*
 * Now for some	declarations
 */

extern char	ltext[];	/* input buffer	*/
extern int	nnonterms;	/* number of nonterminals */
extern int	ntokens;	/* number of terminals */
extern int	nterms;		/* number of terms */
extern int	nalts;		/* number of alternatives */
extern p_start	start;		/* will	contain	startsymbols */
#ifdef NON_CORRECTING
extern int	nsubstarts;	/* number of subparserstarts */
extern p_set	start_firsts;	/* Will contain the union of first sets of
				   startsymbols when -n -s option is on */
#endif
extern int	linecount;	/* line number */
extern int	assval;		/* to create difference	between	literals
				 * and other terminals
				 */
extern p_nont	nonterms;	/* the nonterminal array */
extern p_nont	maxnt;		/* is filled up until here */
extern p_token	tokens;		/* the token array */
extern p_token	maxt;		/* is filled up until here */
extern int	norder, torder;
				/* order of nonterminals in the grammar,
				 * important because actions are copied to
				 * a temporary file in the order in which they
				 * were read
				 */
extern string	e_noopen;	/* Error message string used often */
extern int	verbose;	/* Level of verbosity */
extern int	wflag;		/* warnings? */
extern string	lexical;	/* name of lexical analyser */
extern string	prefix;		/* prefix of externals */
extern string	onerror;	/* name of user error handler */
extern int	ntneeded;	/* ntneeded = 1 if nonterminals are included
				 * in the sets.
				 */
extern int	ntprint;	/* ntprint = 1 if they must be printed too in
				 * the LL.output file (-x option)
				 */
# ifndef NDEBUG
extern int	debug;
# endif /* not NDEBUG */
extern p_file	files,pfile;	/* pointers to file structure.
				 * "files" points to the start of the
				 * list */
extern p_file	maxfiles;
extern string	LLgenid;	/* LLgen identification string */
extern t_token	lextoken;	/* the current token */
extern int	nerrors;
extern string	rec_file, incl_file;
#ifdef NON_CORRECTING
extern string	nc_rec_file, nc_incl_file;
#endif
extern int	low_percentage, high_percentage;
extern int	min_cases_for_jmptable;
extern int	jmptable_option;
#ifdef NON_CORRECTING
extern int	non_corr;
extern int	subpars_sim;
extern p_gram	illegal_gram;
#endif
extern int	strip_grammar;
extern int	in_production;

void error(int lineno,string s,string t);
void warning(int lineno,string s,string t);
void fatal(int lineno,string s,string t);

void conflchecks(void);
void do_compute(void);
int empty(register p_gram p);
int t_safety(int rep, int count, int persistent, int safety);
int t_after(int rep, int count, int outsafety);
void doclose(FILE *f);
void gencode(int argc);
string store(string s);
void name_init(void);
p_gram search(int type,register string str,int option);
void co_reach(void);
void install(string target, string source);
void copyfile(string file);




void UNLINK(string x);
void RENAME(string x,string y);
string libpath(string s);

