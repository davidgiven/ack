/* N O D E   O F   A N   A B S T R A C T   P A R S E T R E E */

/* $Header$ */

struct node {
	struct node *next;
#define nd_left	next
	struct node *nd_right;
	int nd_class;		/* kind of node */
#define Value	1		/* constant */
#define Oper	2		/* binary operator */
#define Uoper	3		/* unary operator */
#define Call	4		/* cast or procedure - or function call */
#define Name	5		/* a qualident */
#define Set	6		/* a set constant */
#define Xset	7		/* a set */
#define Def	8		/* an identified name */
#define Link	11
	struct type *nd_type;	/* type of this node */
	union {
		struct token ndu_token;	/* (Value, Oper, Uoper, Call, Name,
					    Link)
					*/
		arith *ndu_set;		/* pointer to a set constant (Set) */
		struct def *ndu_def;	/* pointer to definition structure for
					   identified name (Def)
					*/
	} nd_val;
#define nd_token	nd_val.ndu_token
#define nd_set		nd_val.ndu_set
#define nd_def		nd_val.ndu_def
#define nd_symb		nd_token.tk_symb
#define nd_lineno	nd_token.tk_lineno
#define nd_filename	nd_token.tk_filename
#define nd_IDF		nd_token.TOK_IDF
#define nd_STR		nd_token.TOK_STR
#define nd_SLE		nd_token.TOK_SLE
#define nd_INT		nd_token.TOK_INT
#define nd_REL		nd_token.TOK_REL
};

/* ALLOCDEF "node" */

extern struct node *MkNode();

#define NULLNODE ((struct node *) 0)
