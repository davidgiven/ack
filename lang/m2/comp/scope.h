/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S C O P E   M E C H A N I S M */

/* $Id$ */

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
	/* struct scope *next; */
	char *sc_name;		/* name of this scope */
	struct def *sc_def;	/* list of definitions in this scope */
	arith sc_off;		/* offsets of variables in this scope */
	char sc_scopeclosed;	/* flag indicating closed or open scope */
	char sc_defmodule;	/* flag set is this scope is from a separate
				   definition module
				*/
	int sc_level;		/* level of this scope */
	struct def *sc_definedby; /* The def structure defining this scope */
	struct node *sc_end;	/* node to remember line number of end of scope */
};

struct scopelist {
	struct scopelist *sc_next;
	struct scope *sc_scope;
	struct scopelist *sc_encl;
	int sc_count;
};

typedef struct scope t_scope;
typedef struct scopelist t_scopelist;

extern t_scope
	*PervasiveScope;

extern t_scopelist
	*CurrVis, *GlobalVis;

#define CurrentScope	(CurrVis->sc_scope)
#define GlobalScope	(GlobalVis->sc_scope)
#define enclosing(x)	((x)->sc_encl)
#define scopeclosed(x)	((x)->sc_scopeclosed)
#define nextvisible(x)	((x)->sc_next)		/* use with scopelists */

t_scope *open_and_close_scope();
