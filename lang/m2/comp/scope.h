/* S C O P E   M E C H A N I S M */

#define OPENSCOPE	0	/* Indicating an open scope */
#define CLOSEDSCOPE	1	/* Indicating a closed scope (module) */

#define SC_CHKFORW	1	/* Check for forward definitions when closing
				   a scope
				*/
#define SC_CHKPROC	2	/* Check for forward procedure definitions
				   when closing a scope
				*/
#define SC_REVERSE	4	/* Reverse list of definitions, to get it
				   back into original order
				*/

struct scope {
	struct scope *next;
	char *sc_name;		/* name of this scope */
	struct def *sc_def;	/* list of definitions in this scope */
	arith sc_off;		/* offsets of variables in this scope */
	char sc_scopeclosed;	/* flag indicating closed or open scope */
	int sc_level;		/* level of this scope */
	struct def *sc_definedby; /* The def structure defining this scope */
};

struct scopelist {
	struct scopelist *next;
	struct scope *sc_scope;
	struct scopelist *sc_encl;
};

extern struct scope
	*PervasiveScope,
	*GlobalScope;

extern struct scopelist
	*CurrVis;

#define CurrentScope	(CurrVis->sc_scope)
#define enclosing(x)	((x)->sc_encl)
#define scopeclosed(x)	((x)->sc_scopeclosed)
#define nextvisible(x)	((x)->next)		/* use with scopelists */
