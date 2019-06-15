/* $Id$ */
#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <system.h>

/* type of arguments expected by each instruction */
#define NOARG  1
#define CST    2
#define CSTOPT 3
#define LAB    4
#define DEFILB 5
#define PNAM   6
#define EXT    7

#define	IDF_TYPE struct id_info
struct id_info {
	struct idf *nextidf;	/* chain all opcodes together */
	int used;		/* is this op used? */
	int startpatt;		/* does it start a pattern? */
	int opcode;		/* opcode of operator */
	int argfmt;		/* how to access pattern argument */
#define id_nextidf	id_user.nextidf
#define id_used		id_user.used
#define id_startpatt	id_user.startpatt
#define id_opcode	id_user.opcode
#define id_argfmt	id_user.argfmt
};
#include <idf_pkg.spec>

struct exp_node {
	int node_type;
	union {
		struct {
			struct exp_node *left;
			struct exp_node *right;
		} interior;
		int val;
	} node_args;
#define exp_left node_args.interior.left
#define exp_right node_args.interior.right
#define leaf_val node_args.val
};

struct mnem_elem {
	struct idf *op_code;
	struct exp_node *arg;	/* optional arg expression if replacement */
};

struct mnem_list {
	struct mnem_list *next;	/* cdr of list */
	struct mnem_elem *elem; /* car of list */
};

struct mnems {
	int m_len;			/* number of mnem's in pattern */
	struct mnem_elem **m_elems;	/* array of mnem's */
};

struct action {
	struct action *next;	/* chain all actions for same state together */
	int linenum;			/* line number in patterns */
	struct exp_node *test;		/* test expression (if any) */
	struct mnems replacement;	/* replacement pattern */
};

struct state {
	struct state *next;	/* chain to next entry for this state */
	struct idf *op;		/* transition on op to.. */
	int goto_state;		/* state 'goto_state' */
};

#define MAXSTATES	2000
#define MAXPATTERN	20

/* Parser globals */
extern struct state	*states[MAXSTATES];
extern struct action	*actions[MAXSTATES];
extern struct mnems	patterns[MAXSTATES];
extern int		numpatterns;	/* Number of patterns */
extern int		higheststate;	/* Highest state yet allocated */
extern struct idf	*ops;		/* Chained list of all ops */
extern int		maxpattern;
extern int		maxreplacement;
extern int		nerrors;
extern FILE		*ofile;

/* Lexical analyser globals */
extern struct idf	*opval;		/* opcode of returned OPCODE*/
extern int		lastintval;	/* value of last integer seen */
extern int		linenum;	/*line number of input file*/

struct exp_node 	*mknode(int, struct exp_node *, struct exp_node *);
struct exp_node 	*mkleaf(int, int);
struct exp_node 	*combinetests(struct exp_node *, struct exp_node *);
struct mnem_list 	*addelem(struct mnem_list *, struct idf *, struct exp_node *);
struct mnem_elem 	**constructlist(struct mnem_list *, int);
void findworst(struct mnems,struct mnems);

void outputincalls(void);
void outputnopt(void);
void initlex(void);
void findfail(int, int *, int *, int *);


#endif /* PARSER_H_ */
