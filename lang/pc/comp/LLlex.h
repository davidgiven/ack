/* T O K E N   D E S C R I P T O R   D E F I N I T I O N */

/* Structure to store a string constant
*/
struct string {
	arith s_length;			/* length of a string */
	char *s_str;			/* the string itself */
	label s_lab;			/* data label of string */
};

/* Structure to store a real constant
*/
struct real {
	char *r_real;			/* string representation of real */
	struct real *r_inverse;		/* the inverse of this real */
	label r_lab;			/* data label of real */
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
		struct def *tk_def;	/* only used in parse tree node */
		arith *tk_set;		/* only used in parse tree node */
		label tk_lab;		/* only used in parse tree node */
	} tk_data;
};

#define TOK_IDF	tk_data.tk_idf
#define TOK_STR	tk_data.tk_str->s_str
#define TOK_SLE tk_data.tk_str->s_length
#define TOK_SLA	tk_data.tk_str->s_lab
#define TOK_INT	tk_data.tk_int
#define TOK_REL	tk_data.tk_real->r_real
#define TOK_RIV	tk_data.tk_real->r_inverse
#define TOK_RLA	tk_data.tk_real->r_lab

extern struct token dot, aside;
extern struct type *toktype, *asidetype;
extern int tokenseen;

#define	ASIDE	aside.tk_symb
