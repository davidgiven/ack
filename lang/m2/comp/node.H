/* N O D E   O F   A N   A B S T R A C T   P A R S E T R E E */

/* $Header$ */

struct node {
	struct node *next;
#define nd_left	next
	struct node *nd_right;
	int nd_class;		/* kind of node */
#define Value	1		/* idf or constant */
#define Oper	2		/* binary operator */
#define Uoper	3		/* unary operator */
#define Call	4		/* cast or procedure - or function call */
#define Link	5
	struct type *nd_type;	/* type of this node */
	struct token nd_token;
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
