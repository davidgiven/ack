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
 * extern.h $Header$
 * Miscellanious constants and
 * some variables that are visible in more than one file
 */

/*
 * options for the identifier search routine
 */
# define JUSTLOOKING	0
# define ENTERING	1
# define BOTH		2

/*
 * Now for some	declarations
 */

extern char	ltext[];	/* input buffer	*/
extern int	nnonterms;	/* number of nonterminals */
extern int	nterminals;	/* number of terminals */
extern p_start	start;		/* will	contain	startsymbols */
extern int	linecount;	/* line number */
extern int	assval;		/* to create difference	between	literals
				 * and other terminals
				 */
extern t_nont	nonterms[];	/* the nonterminal array */
extern p_nont	maxnt;		/* is filled up until here */
extern int	order[];	/* order of nonterminals in the grammar,
				 * important because actions are copied to
				 * a temporary file in the order in which they
				 * were read
				 */
extern int	*maxorder;	/* will contain &order[nnonterms] */
extern t_entry	h_entry[];	/* terminal and nonterminal entrys,
				 * first NTERMINAL entrys reserved
				 * for terminals
				 */
extern p_entry	max_t_ent;	/* will contain &h_entry[nterminals] */
# define	min_nt_ent	&h_entry[NTERMINALS]
extern string	pentry[];	/* pointers to various allocated things */
extern string	e_noopen;	/* Error message string used often */
extern int	verbose;	/* Level of verbosity */
extern string	lexical;	/* name of lexical analyser */
extern int	ntneeded;	/* ntneeded = 1 if nonterminals are included
				 * in the sets.
				 */
extern int	ntprint;	/* ntprint = 1 if they must be printed too in
				 * the LL.output file (-x option)
				 */
# ifndef NDEBUG
extern int	debug;
# endif not NDEBUG
extern p_file	files,pfile;	/* pointers to file structure.
				 * "files" points to the start of the
				 * list */
extern string	LLgenid;	/* LLgen identification string */
extern t_token	lextoken;	/* the current token */
extern int	nerrors;
extern int	fflag;		/* Enable compiler to generate jump tables
				 * for switches?
				 */
