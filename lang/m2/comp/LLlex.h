/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* T O K E N   D E S C R I P T O R   D E F I N I T I O N */

/* $Header$ */

#include "real.h"

/* Structure to store a string constant
*/
struct string {
	unsigned s_length;		/* length of a string */
	char *s_str;			/* the string itself */
};

/* Token structure. Keep it small, as it is part of a parse-tree node
*/
struct token	{
	short tk_symb;			/* token itself	*/
	unsigned short tk_lineno;	/* linenumber on which it occurred */
	union {
		struct idf *tk_idf;	/* IDENT	*/
		struct string *tk_str;	/* STRING	*/
		arith tk_int;		/* INTEGER	*/
		struct real *tk_real;	/* REAL		*/
		arith *tk_set;		/* only used in parse tree node */
		struct def *tk_def;	/* only used in parse tree node */
	} tk_data;
};

typedef struct token	t_token;

#define TOK_IDF	tk_data.tk_idf
#define TOK_SSTR tk_data.tk_str
#define TOK_STR	tk_data.tk_str->s_str
#define TOK_SLE tk_data.tk_str->s_length
#define TOK_INT	tk_data.tk_int
#define TOK_REAL tk_data.tk_real
#define TOK_RSTR tk_data.tk_real->r_real
#define TOK_RVAL tk_data.tk_real->r_val

extern t_token dot, aside;
extern struct type *toktype;

#define DOT	dot.tk_symb
#define ASIDE	aside.tk_symb
