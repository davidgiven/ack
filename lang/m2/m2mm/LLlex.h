/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* T O K E N   D E S C R I P T O R   D E F I N I T I O N */

/* stripped down version of the one in the Modula-2 compiler */

/* $Id$ */

/* Token structure. Keep it small, as it is part of a parse-tree node
*/
struct token	{
	short tk_symb;			/* token itself	*/
	unsigned short tk_lineno;	/* linenumber on which it occurred */
	struct idf *tk_idf;		/* IDENT	*/
};

#define TOK_IDF	tk_idf

extern struct token	dot, aside;
extern int		ForeignFlag;

#define DOT	dot.tk_symb
#define ASIDE	aside.tk_symb
