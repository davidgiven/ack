/* $Header$ */
/* DEFINITION OF DECLARATOR DESCRIPTORS */

/*	A 'declarator' consists of an idf and a linked list of
	language-defined unary operations: *, [] and (), called
	decl_unary's.
*/

struct declarator	{
	struct declarator *next;
	struct idf *dc_idf;
	struct decl_unary *dc_decl_unary;
	struct idstack_item *dc_fparams;	/* params for function	*/
};


/* allocation definitions of struct declarator */
/* ALLOCDEF "declarator" */
extern char *st_alloc();
extern struct declarator *h_declarator;
#define	new_declarator() ((struct declarator *) \
		st_alloc((char **)&h_declarator, sizeof(struct declarator)))
#define	free_declarator(p) st_free(p, h_declarator, sizeof(struct declarator))


#define	NO_PARAMS ((struct idstack_item *) 0)

struct decl_unary	{
	struct decl_unary *next;
	int du_fund;			/* POINTER, ARRAY or FUNCTION	*/
	arith du_count;			/* for ARRAYs only	*/
};


/* allocation definitions of struct decl_unary */
/* ALLOCDEF "decl_unary" */
extern char *st_alloc();
extern struct decl_unary *h_decl_unary;
#define	new_decl_unary() ((struct decl_unary *) \
		st_alloc((char **)&h_decl_unary, sizeof(struct decl_unary)))
#define	free_decl_unary(p) st_free(p, h_decl_unary, sizeof(struct decl_unary))


extern struct type *declare_type();
extern struct declarator null_declarator;
