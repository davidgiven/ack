/* S C O P E   M E C H A N I S M */

/* $Header$ */

#define OPENSCOPE	0	/* Indicating an open scope */
#define CLOSEDSCOPE	1	/* Indicating a closed scope (module) */

#define SC_CHKFORW	1	/* Check for forward definitions when closing
				   a scope
				*/
#define SC_CHKPROC	2	/* Check for forward procedure definitions
				   when closing a scope
				*/

struct scope {
	struct scope *next;
	struct forwards *sc_forw;
	char *sc_name;		/* name of this scope */
	struct def *sc_def;	/* list of definitions in this scope */
	arith sc_off;		/* offsets of variables in this scope */
	char sc_scopeclosed;	/* flag indicating closed or open scope */
	int sc_level;		/* level of this scope */
};

extern struct scope
	*CurrentScope,
	*PervasiveScope,
	*GlobalScope;

#define enclosing(x)	((x)->next)
#define scopeclosed(x)	((x)->sc_scopeclosed)
#define nextvisible(x)	(scopeclosed(x) ? PervasiveScope : enclosing(x))
