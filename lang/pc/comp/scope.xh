/* S C O P E   M E C H A N I S M */

struct scope {
	struct scope *next;
	struct def *sc_def;	/* list of definitions in this scope */
	int sc_level;		/* level of this scope */
	arith sc_off;		/* offsets of variables in this scope */
	struct node *sc_lablist;/* list of labels in this scope, to speed
				   up label handling
				*/
};

/* ALLOCDEF "scope" 10 */

struct scopelist {
	struct scopelist *next;
	struct scope *sc_scope;
	int sc_count;
};

/* ALLOCDEF "scopelist" 10 */

extern struct scope
	*GlobalScope,
	*PervasiveScope,
	*BlockScope;

extern struct scopelist
	*CurrVis;

#define	CurrentScope	(CurrVis->sc_scope)
#define	nextvisible(x)	((x)->next)		/* use with scopelists */
