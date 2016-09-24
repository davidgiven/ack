#ifndef BURG_INCLUDED
#define BURG_INCLUDED

/* $Id$ */
/* iburg.c: */
extern char* stringf(char* fmt, ...);

typedef struct stringlist* Stringlist;
struct stringlist {
    const char* payload;
    Stringlist next;
};
extern Stringlist pushstring(const char* data, Stringlist list);

typedef enum
{
	TERM = 1,
	NONTERM
} Kind;
typedef struct rule* Rule;
typedef struct term* Term;
struct term
{ /* terminals: */
	char* name; /* terminal name */
	Kind kind; /* TERM */
	int esn; /* external symbol number */
	int arity; /* operator arity */
	Term link; /* next terminal in esn order */
	Rule rules; /* rules whose pattern starts with term */
};

typedef struct nonterm* Nonterm;
struct nonterm
{ /* non-terminals: */
	char* name; /* non-terminal name */
	Kind kind; /* NONTERM */
	int number; /* identifying number */
	int lhscount; /* # times nt appears in a rule lhs */
	int reached; /* 1 iff reached from start non-terminal */
	Rule rules; /* rules w/non-terminal on lhs */
	Rule chain; /* chain rules w/non-terminal on rhs */
	Nonterm link; /* next terminal in number order */
};
extern Nonterm nonterm(const char* id);
extern Term term(const char* id, int esn);

typedef struct tree* Tree;
struct tree
{ /* tree patterns: */
	void* op;           /* a terminal or non-terminal */
	const char* label;  /* user label for this node */
	Tree left, right;   /* operands */
	int nterms;         /* number of terminal nodes in this tree */
};
extern Tree tree(const char* op, const char* label, Tree left, Tree right);

struct rule
{ /* rules: */
	Nonterm lhs;     /* lefthand side non-terminal */
	Tree pattern;    /* rule pattern */
	int ern;         /* external rule number */
	int packed;      /* packed external rule number */
	int cost;        /* associated cost */
	Rule link;       /* next rule in ern order */
	Rule next;       /* next rule with same pattern root */
	Rule chain;      /* next chain rule with same rhs */
	Rule decode;     /* next rule with same lhs */
	Rule kids;       /* next rule with same burm_kids pattern */
	Stringlist when; /* C predicate string */
};
extern Rule rule(char* id, Tree pattern, int ern);
extern int maxcost; /* maximum cost */

/* gram.y: */
void yyerror(char* fmt, ...);
int yyparse(void);
void yywarn(char* fmt, ...);
extern int errcnt;
extern FILE* infp;
extern FILE* outfp;

/* Stupid flex imports --- why mo header file? */

extern FILE* yyin;
extern int yylineno;

extern void printlineno(void);

#include "mcgg.h"

#endif
