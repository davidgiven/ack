/* T O K E N   D E S C R I P T O R   D E F I N I T I O N */

/* $Header$ */

struct string {
	unsigned int s_length;	/* length of a string */
	char *s_str;		/* the string itself */
};

struct token	{
	int tk_symb;		/* token itself	*/
	char *tk_filename;	/* filename in which it occurred */
	int tk_lineno;		/* linenumber on which it occurred */
	union {
		struct idf *tk_idf;	/* IDENT	*/
		struct string *tk_str;	/* STRING	*/
		arith tk_int;		/* INTEGER	*/
		char *tk_real;		/* REAL		*/
		arith *tk_set;		/* only used in parse tree node */
		struct def *tk_def;	/* only used in parse tree node */
	} tk_data;
};

#define TOK_IDF	tk_data.tk_idf
#define TOK_STR	tk_data.tk_str->s_str
#define TOK_SLE tk_data.tk_str->s_length
#define TOK_INT	tk_data.tk_int
#define TOK_REL	tk_data.tk_real

extern struct token dot, aside;
extern struct type *numtype;

#define DOT	dot.tk_symb
#define ASIDE	aside.tk_symb
