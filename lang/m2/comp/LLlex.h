/*	Token Descriptor Definition	*/

/* $Header$ */

struct token	{
	int tk_symb;		/* token itself	*/
	union {
		struct idf *tk_idf;	/* IDENT	*/
		char *tk_str;		/* STRING	*/
		struct {		/* INTEGER	*/
			int tk_type;	/* type	*/
			long tk_value;	/* value	*/
		} tk_int;
		char *tk_real;		/* REAL		*/
	} tk_data;
};

#define TOK_IDF	tk_data.tk_idf
#define TOK_STR	tk_data.tk_str
#define TOK_ITP	tk_data.tk_int.tk_type
#define TOK_INT	tk_data.tk_int.tk_value
#define TOK_REL	tk_data.tk_real

extern struct token dot, aside;

#define DOT	dot.tk_symb
#define ASIDE	aside.tk_symb
