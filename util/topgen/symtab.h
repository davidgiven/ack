/* s y m t a b . h
 *
 * Contains a structure declaration for a symbol table that is a
 * binary tree
 */
struct symtab {
	char *s_name;			/* The name of the symbol */
	int s_num;			/* it's number */
	struct symtab *s_left, *s_right;
};

extern struct symtab *idtable,		/* table for variables */
		     *deftable;		/* table for tunable defines */
struct symtab *findident();

/* Options to "findident" */
#define LOOKING		1
#define ENTERING	0
