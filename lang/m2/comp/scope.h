/* S C O P E   M E C H A N I S M */

/* $Header$ */

#define OPENSCOPE	0	/* Indicating an open scope */
#define CLOSEDSCOPE	1	/* Indicating a closed scope (module) */

struct scope {
	struct scope *next;
	int sc_scope;		/* The scope number. Scope number 0 indicates
				   both the pervasive scope and the end of a
				   visibility range
				*/
};

extern struct scope
	*CurrentScope;

#define nextvisible(x)	((x)->sc_scope ? (x)->next : (struct scope *) 0)
#define scopeclosed(x)	((x)->next->sc_scope == 0)
