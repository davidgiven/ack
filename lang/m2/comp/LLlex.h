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

union tk_attr {
	struct string *tk_str;
	arith tk_int;
	struct real *tk_real;
	struct {
		union {
			arith *tky_set;
			struct idf *tky_idf;
			struct def *tky_def;
		} tk_yy;
		struct node *tky_next;
	} tk_y;
	struct {
		struct node *tkx_left, *tkx_right;
	} tk_x;
};
#define tk_left	tk_x.tkx_left
#define tk_right tk_x.tkx_right
#define tk_next	tk_y.tky_next
#define tk_idf	tk_y.tk_yy.tky_idf
#define tk_def	tk_y.tk_yy.tky_def
#define tk_set	tk_y.tk_yy.tky_set

/* Token structure. Keep it small, as it is part of a parse-tree node
*/
struct token	{
	short tk_symb;			/* token itself	*/
	unsigned short tk_lineno;	/* linenumber on which it occurred */
	union tk_attr tk_data;
};

typedef struct token	t_token;

#define TOK_IDF		tk_data.tk_idf
#define TOK_SSTR	tk_data.tk_str
#define TOK_STR		tk_data.tk_str->s_str
#define TOK_SLE		tk_data.tk_str->s_length
#define TOK_INT		tk_data.tk_int
#define TOK_REAL	tk_data.tk_real
#define TOK_RSTR	tk_data.tk_real->r_real
#define TOK_RVAL	tk_data.tk_real->r_val

extern t_token dot, aside;
extern struct type *toktype;

#define DOT	dot.tk_symb
#define ASIDE	aside.tk_symb
