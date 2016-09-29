#ifndef BURG_INCLUDED
#define BURG_INCLUDED

#include "stringlist.h"

extern char* stringf(char* fmt, ...);

typedef enum
{
	TERM = 1,
	NONTERM,
	REG,
	REGCLASS
} Kind;
typedef struct rule* Rule;
typedef struct term* Term;

struct reg
{
	const char* name;      /* register name */
	Kind kind;             /* REG */
	int number;            /* identifying number */
	uint32_t classes;      // bitfield of classes */
	struct reg* link;      /* next in list */
};

struct regclass
{
	const char* name;      /* class name */
	Kind kind;             /* REGCLASS */
	int number;            /* identifying number */
	struct regclass* link; /* next in list */
};

extern struct reg* makereg(const char* name);
extern void addregclass(struct reg* reg, const char* regclass);
extern struct regclass* getregclass(const char* name);

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
	char* name;       /* non-terminal name */
	Kind kind;        /* NONTERM */
	int number;       /* identifying number */
	int lhscount;     /* # times nt appears in a rule lhs */
	int reached;      /* 1 iff reached from start non-terminal */
	Rule rules;       /* rules w/non-terminal on lhs */
	Rule chain;       /* chain rules w/non-terminal on rhs */
	Nonterm link;     /* next terminal in number order */
	bool is_fragment; /* these instructions are all fragments */
	struct regclass* allocate; /* allocate this kind of register */
};
extern void* lookup(const char* name);
extern Nonterm nonterm(const char* id, bool allocate);
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
	Nonterm lhs;             /* lefthand side non-terminal */
	Tree pattern;            /* rule pattern */
	int lineno;              /* line number where allocated */
	int ern;                 /* external rule number */
	int packed;              /* packed external rule number */
	int cost;                /* associated cost */
	Rule link;               /* next rule in ern order */
	Rule next;               /* next rule with same pattern root */
	Rule chain;              /* next chain rule with same rhs */
	Rule decode;             /* next rule with same lhs */
	Rule kids;               /* next rule with same burm_kids pattern */
	struct stringlist when;  /* C predicate string */
	struct stringlist code;  /* compiler output code strings */
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
