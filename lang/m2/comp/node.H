/* N O D E   O F   A N   A B S T R A C T   P A R S E T R E E */

/* $Header$ */

struct node {
	struct node *next;
#define nd_left	next
	struct node *nd_right;
	int nd_class;		/* kind of node */
#define Value	0		/* constant */
#define Arrsel  1		/* array selection */
#define Oper	2		/* binary operator */
#define Uoper	3		/* unary operator */
#define Arrow	4		/* ^ construction */
#define Call	5		/* cast or procedure - or function call */
#define Name	6		/* an identifier */
#define Set	7		/* a set constant */
#define Xset	8		/* a set */
#define Def	9		/* an identified name */
#define Stat	10		/* a statement */
#define Link	11
	struct type *nd_type;	/* type of this node */
	struct token nd_token;
#define nd_set		nd_token.tk_data.tk_set
#define nd_def		nd_token.tk_data.tk_def
#define nd_lab		nd_token.tk_data.tk_lab
#define nd_symb		nd_token.tk_symb
#define nd_lineno	nd_token.tk_lineno
#define nd_IDF		nd_token.TOK_IDF
#define nd_STR		nd_token.TOK_STR
#define nd_SLE		nd_token.TOK_SLE
#define nd_INT		nd_token.TOK_INT
#define nd_REL		nd_token.TOK_REL
};

/* ALLOCDEF "node" */

extern struct node *MkNode(), *MkLeaf();

#define NULLNODE ((struct node *) 0)

#define DESIGNATOR 1
#define HASSELECTORS 2
#define VARIABLE 4
#define VALUE 8

#define	IsCast(lnd)	((lnd)->nd_class == Def && is_type((lnd)->nd_def))
#define	IsProcCall(lnd)	((lnd)->nd_type->tp_fund == T_PROCEDURE)
