#include <assert.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "iburg.h"
#include "ircodes.h"

static char rcsid[] = "$Id$";

int maxcost = SHRT_MAX / 2;

static char* prefix = "burm";
static int Tflag = 1; /* tracing */
static int ntnumber = 0;
static Nonterm start = 0;
static struct reg* regs = NULL;
static struct regclass* regclasses = NULL;
static Term terms;
static Nonterm nts;
static Rule rules;
static int nrules;

static void print(char* fmt, ...);
static void ckreach(Nonterm p);
static void registerterminals(void);
static void emitclosure(Nonterm nts);
static void emitcost(Tree t, char* v);
static void emitcostcalc(Rule r);
static void emitdefs(Nonterm nts, int ntnumber);
static void emitfuncs(void);
static void emitheader(void);
static void emitinsndata(Rule rules);
static void emitkids(Rule rules, int nrules);
static void emitlabel(Nonterm start);
static void emitleaf(Term p, int ntnumber);
static void emitnts(Rule rules, int nrules);
static void emitpredicatedefinitions(Rule rules);
static void emitrecord(char* pre, Rule r, int cost);
static void emitregisterclasses(struct regclass* rc);
static void emitregisters(struct reg* regs);
static void emitrule(Nonterm nts);
static void emitstate(Term terms, Nonterm start, int ntnumber);
static void emitstring(Rule rules);
static void emitstruct(Nonterm nts, int ntnumber);
static void emitterms(Term terms);
static void emittest(Tree t, char* v, char* suffix);

extern int yy_flex_debug;

int main(int argc, char* argv[])
{
	int c, i;
	Nonterm p;

	#if 0
	extern int yydebug;
	yydebug = 1;
	#endif

	infp = stdin;
	outfp = stdout;
	yy_flex_debug = 0;

	for (;;)
	{
		int opt = getopt(argc, argv, "p:i:o:yf");
		if (opt == -1)
			break;

		switch (opt)
		{
			case 'p':
				prefix = optarg;
				break;

			case 'i':
				infp = fopen(optarg, "r");
				if (!infp)
				{
					yyerror("cannot open input file: %s\n", strerror(errno));
					exit(1);
				}
				break;

			case 'o':
				outfp = fopen(optarg, "w");
				if (!outfp)
				{
					yyerror("cannot open output file: %s\n", strerror(errno));
					exit(1);
				}
				break;

			case 'y':
			{
				extern int yydebug;
				yydebug = 1;
				break;
			}

			case 'f':
			{
				yy_flex_debug = 1;
				break;
			}

			default:
				yyerror("usage: %s [-p prefix] < input > output\n", argv[0]);
				exit(1);
		}
	}
				
	emitheader();
	registerterminals();

	start = nonterm("stmt", true);

	yyin = infp;
	yyparse();

	if (start)
		ckreach(start);
	for (p = nts; p; p = p->link)
		if (!p->reached)
			yyerror("can't reach non-terminal `%s'\n", p->name);

	emitregisterclasses(regclasses);
	emitregisters(regs);
	emitdefs(nts, ntnumber);
	emitstruct(nts, ntnumber);
	emitnts(rules, nrules);
	emitterms(terms);
	emitstring(rules);
	emitrule(nts);
	emitclosure(nts);
	emitpredicatedefinitions(rules);
	emitinsndata(rules);
	if (start)
		emitstate(terms, start, ntnumber);
	print("#ifdef STATE_LABEL\n");
	if (start)
		emitlabel(start);
	emitkids(rules, nrules);
	emitfuncs();
	print("#endif\n");
	print("#include \"mcgg_generated_footer.h\"\n");
	return errcnt > 0;
}

/* stringf - format and save a string */
char* stringf(char* fmt, ...)
{
    int n;
    char* p;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);

    p = malloc(n);
    if (!p)
        return NULL;

    va_start(ap, fmt);
    vsnprintf(p, n, fmt, ap);
    va_end(ap);

    return p;
}

static void registerterminal(const struct ir_data* data, int iropcode, int size)
{
	const char* s = (size == 0) ? data->name : stringf("%s%d", data->name, size);
	int esn = ir_to_esn(iropcode, size);

	term(s, esn);
}

static void registerterminals(void)
{
	int i;

	for (i=0; i<IR__COUNT; i++)
	{
		if (ir_data[i].flags & IRF_SIZED)
		{
			registerterminal(&ir_data[i], i, 1);
			registerterminal(&ir_data[i], i, 2);
			registerterminal(&ir_data[i], i, 4);
			registerterminal(&ir_data[i], i, 8);
		}
		else
			registerterminal(&ir_data[i], i, 0);
	}
}

struct entry
{
	union
	{
		const char* name;
		struct term t;
		struct nonterm nt;
		struct reg r;
		struct regclass rc;
	} sym;
	struct entry* link;
} * table[211];
#define HASHSIZE (sizeof table / sizeof table[0])

/* hash - return hash number for str */
static unsigned hash(const char* str)
{
	unsigned h = 0;

	while (*str)
		h = (h << 1) + *str++;
	return h;
}

/* lookup - lookup symbol name */
void* lookup(const char* name)
{
	struct entry* p = table[hash(name) % HASHSIZE];

	for (; p; p = p->link)
		if (strcmp(name, p->sym.name) == 0)
			return &p->sym;
	return 0;
}

/* install - install symbol name */
static void* install(const char* name)
{
	struct entry* p = calloc(1, sizeof *p);
	int i = hash(name) % HASHSIZE;

	p->sym.name = name;
	p->link = table[i];
	table[i] = p;
	return &p->sym;
}

struct reg* makereg(const char* id)
{
	struct reg* p = lookup(id);
	struct reg** q = &regs;
	static int number = 1;

	if (p)
		yyerror("redefinition of '%s'", id);
	p = install(id);
	p->kind = REG;
	p->number = number++;

	while (*q && (*q)->number < p->number)
		q = &(*q)->link;
	assert(*q == 0 || (*q)->number != p->number);
	p->link = *q;
	*q = p;
	return p;
}

void addregclass(struct reg* reg, const char* id)
{
	struct regclass* p = lookup(id);
	struct regclass** q = &regclasses;
	static int number = 1;

	if (p && (p->kind != REGCLASS))
		yyerror("redefinition of '%s' as something else\n", id);
	if (!p)
	{
		p = install(id);
		p->kind = REGCLASS;
		p->number = number++;

		while (*q && (*q)->number < p->number)
			q = &(*q)->link;
		assert(*q == 0 || (*q)->number != p->number);
		p->link = *q;
		*q = p;
	}
}

struct regclass* getregclass(const char* id)
{
	struct regclass* p = lookup(id);
	if (!p || (p->kind != REGCLASS))
		yyerror("'%p' is not the name of a register class");
	return p;
}

/* nonterm - create a new terminal id, if necessary */
Nonterm nonterm(const char* id, bool allocate)
{
	Nonterm p = lookup(id);
	Nonterm* q = &nts;

	if (p && p->kind == NONTERM)
		return p;
	if (p)
		yyerror("redefinition of '%s' as something else\n", id);
	if (!allocate)
		yyerror("'%s' has not been declared\n", id);

	p = install(id);
	p->kind = NONTERM;
	p->number = ++ntnumber;
	if (p->number == 1)
		start = p;
	while (*q && (*q)->number < p->number)
		q = &(*q)->link;
	assert(*q == 0 || (*q)->number != p->number);
	p->link = *q;
	*q = p;
	return p;
}

/* term - create a new terminal id with external symbol number esn */
Term term(const char* id, int esn)
{
	Term p = lookup(id);
	Term* q = &terms;

	if (p)
		yyerror("redefinition of '%s'\n", id);

	p = install(id);
	p->kind = TERM;
	p->esn = esn;
	p->arity = -1;
	while (*q && (*q)->esn < p->esn)
		q = &(*q)->link;
	if (*q && (*q)->esn == p->esn)
		yyerror("duplicate external symbol number `%s=%d'\n",
		    p->name, p->esn);
	p->link = *q;
	*q = p;

	if (esn != -1)
		print("enum { %s = %d };\n", id, esn);
	return p;
}

/* tree - create & initialize a tree node with the given fields */
Tree tree(const char* id, const char* label, Tree left, Tree right)
{
	Tree t = calloc(1, sizeof *t);
	Term p = lookup(id);
	int arity = 0;

	if (left && right)
		arity = 2;
	else if (left)
		arity = 1;
	if (p == NULL && arity > 0)
	{
		yyerror("undefined terminal `%s'\n", id);
		p = term(id, -1);
	}
	else if (p == NULL && arity == 0)
		p = (Term)nonterm(id, false);
	else if (p && p->kind == NONTERM && arity > 0)
	{
		yyerror("`%s' is a non-terminal\n", id);
		p = term(id, -1);
	}
	if (p->kind == TERM && p->arity == -1)
		p->arity = arity;
	if (p->kind == TERM && arity != p->arity)
		yyerror("inconsistent arity for terminal `%s'\n", id);
	t->op = p;
	t->label = label;
	t->nterms = p->kind == TERM;
	if (t->left = left)
		t->nterms += left->nterms;
	if (t->right = right)
		t->nterms += right->nterms;
	return t;
}

/* rule - create & initialize a rule with the given fields */
Rule rule(char* id, Tree pattern, int ern)
{
	Rule r = calloc(1, sizeof *r);
	Rule *q;
	Term p = pattern->op;

	nrules++;
	r->lineno = yylineno;
	r->lhs = nonterm(id, false);
	r->packed = ++r->lhs->lhscount;
	for (q = &r->lhs->rules; *q; q = &(*q)->decode)
		;
	*q = r;
	r->pattern = pattern;
	r->ern = ern;
	if (p->kind == TERM)
	{
		r->next = p->rules;
		p->rules = r;
	}
	else if (pattern->left == NULL && pattern->right == NULL)
	{
		Nonterm p = pattern->op;
		r->chain = p->chain;
		p->chain = r;
	}
	for (q = &rules; *q && (*q)->ern < r->ern; q = &(*q)->link)
		;
	if (*q && (*q)->ern == r->ern)
		yyerror("duplicate external rule number `%d'\n", r->ern);
	r->link = *q;
	*q = r;
	return r;
}

/* print - formatted output */
static void print(char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	for (; *fmt; fmt++)
		if (*fmt == '%')
			switch (*++fmt)
			{
				case 'd':
					fprintf(outfp, "%d", va_arg(ap, int));
					break;
				case 's':
					fputs(va_arg(ap, char*), outfp);
					break;
				case 'P':
					fprintf(outfp, "%s_", prefix);
					break;
				case 'T':
				{
					Tree t = va_arg(ap, Tree);
					print("%S", t->op);
					if (t->left && t->right)
						print("(%T,%T)", t->left, t->right);
					else if (t->left)
						print("(%T)", t->left);
					break;
				}
				case 'R':
				{
					Rule r = va_arg(ap, Rule);
					print("%S: %T", r->lhs, r->pattern);
					break;
				}
				case 'S':
					fputs(va_arg(ap, Term)->name, outfp);
					break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				{
					int n = *fmt - '0';
					while (n-- > 0)
						putc('\t', outfp);
					break;
				}
				default:
					putc(*fmt, outfp);
					break;
			}
		else
			putc(*fmt, outfp);
	va_end(ap);
}

void printlineno(void)
{
	//print("#line %d\n", yylineno);
}

/* reach - mark all non-terminals in tree t as reachable */
static void reach(Tree t)
{
	Nonterm p = t->op;

	if (p->kind == NONTERM)
		if (!p->reached)
			ckreach(p);
	if (t->left)
		reach(t->left);
	if (t->right)
		reach(t->right);
}

/* ckreach - mark all non-terminals reachable from p */
static void ckreach(Nonterm p)
{
	Rule r;

	p->reached = 1;
	for (r = p->rules; r; r = r->decode)
		reach(r->pattern);
}

static void emitregisterclasses(struct regclass* rc)
{
	int k = 0;
	print("const char* %Pregister_class_names[] = {\n");
	while (rc)
	{
		for (; k < rc->number; k++)
			print("%1NULL,\n");
		k++;

		print("%1\"%s\",\n", rc->name);
		rc = rc->link;
	}
	print("};\n\n");
}

static void emitregisters(struct reg* r)
{
	int k = 0;
	print("const char* %Pregister_names[] = {\n");
	while (r)
	{
		for (; k < r->number; k++)
			print("%1NULL,\n");
		k++;

		print("%1\"%s\",\n", r->name);
		r = r->link;
	}
	print("};\n\n");
}

/* emitcase - emit one case in function state */
static void emitcase(Term p, int ntnumber)
{
	Rule r;

	if (!p->rules)
		return;

	print("%1case %d: /* %S */\n", p->esn, p);
	switch (p->arity)
	{
		case 0:
		case -1:
			break;
		case 1:
			print("%2assert(l);\n");
			break;
		case 2:
			print("%2assert(l && r);\n");
			break;
		default:
			assert(0);
	}
	for (r = p->rules; r; r = r->next)
	{
		switch (p->arity)
		{
			case 0:
			case -1:
				print("%2{%1/* %R */\n%3c = ", r);
				emitcostcalc(r);
				break;
			case 1:
				if (r->pattern->nterms > 1)
				{
					print("%2if (%1/* %R */\n", r);
					emittest(r->pattern->left, "l", " ");
					print("%2) {\n%3c = ");
				}
				else
				{
					print("%2{%1/* %R */\n%3c = ", r);
				}
				emitcostcalc(r);
				emitcost(r->pattern->left, "l");
				break;
			case 2:
				if (r->pattern->nterms > 1)
				{
					print("%2if (%1/* %R */\n", r);
					emittest(r->pattern->left, "l",
					    r->pattern->right->nterms ? " && " : " ");
					emittest(r->pattern->right, "r", " ");
					print("%2) {\n%3c = ");
				}
				else
				{
					print("%2{%1/* %R */\n%3c = ", r);
				}
				emitcostcalc(r);
				emitcost(r->pattern->left, "l");
				emitcost(r->pattern->right, "r");
				break;
			default:
				assert(0);
		}
		print("%d;\n", r->cost);
		emitrecord("\t\t\t", r, 0);
		print("%2}\n");
	}
	print("%2break;\n");
}

/* emitclosure - emit the closure functions */
static void emitclosure(Nonterm nts)
{
	Nonterm p;

	for (p = nts; p; p = p->link)
		if (p->chain)
			print("static void %Pclosure_%S(struct %Pstate *, int);\n", p);
	print("\n");
	for (p = nts; p; p = p->link)
		if (p->chain)
		{
			Rule r;
			print("static void %Pclosure_%S(struct %Pstate *p, int c) {\n", p);
			for (r = p->chain; r; r = r->chain)
				emitrecord("\t", r, r->cost);
			print("}\n\n");
		}
}

/* emitcost - emit cost computation for tree t */
static void emitcost(Tree t, char* v)
{
	Nonterm p = t->op;

	if (p->kind == TERM)
	{
		if (t->left)
			emitcost(t->left, stringf("%s->left", v));
		if (t->right)
			emitcost(t->right, stringf("%s->right", v));
	}
	else
		print("%s->cost[%P%S_NT] + ", v, p);
}

/* emitdefs - emit non-terminal defines and data structures */
static void emitdefs(Nonterm nts, int ntnumber)
{
	Nonterm p;

	for (p = nts; p; p = p->link)
		print("#define %P%S_NT %d\n", p, p->number);
	print("static const int %Pmax_nt = %d;\n\n", ntnumber);
	print("const char *%Pntname[] = {\n%10,\n");
	for (p = nts; p; p = p->link)
		print("%1\"%S\",\n", p);
	print("%10\n};\n\n");
}

/* emitfuncs - emit functions to access node fields */
static void emitfuncs(void)
{
	print("int %Pop_label(NODEPTR_TYPE p) {\n"
	      "%1%Passert(p, PANIC(\"NULL tree in %Pop_label\\n\"));\n"
	      "%1return OP_LABEL(p);\n}\n\n");
	print("STATE_TYPE %Pstate_label(NODEPTR_TYPE p) {\n"
	      "%1%Passert(p, PANIC(\"NULL tree in %Pstate_label\\n\"));\n"
	      "%1return STATE_LABEL(p);\n}\n\n");
	print("NODEPTR_TYPE %Pchild(NODEPTR_TYPE p, int index) {\n"
	      "%1%Passert(p, PANIC(\"NULL tree in %Pchild\\n\"));\n"
	      "%1switch (index) {\n%1case 0:%1return LEFT_CHILD(p);\n"
	      "%1case 1:%1return RIGHT_CHILD(p);\n%1}\n"
	      "%1%Passert(0, PANIC(\"Bad index %%d in %Pchild\\n\", index));\n%1return 0;\n}\n\n");
}

/* emitheader - emit initial definitions */
static void emitheader(void)
{
	print("#include \"mcgg_generated_header.h\"\n");
	if (Tflag)
		print("static NODEPTR_TYPE %Pnp;\n\n");
}

/* computekids - compute paths to kids in tree t */
static char* computekids(Tree t, char* v, char* bp, int* ip)
{
	Term p = t->op;

	if (p->kind == NONTERM)
	{
		sprintf(bp, "\t\tkids[%d] = %s;\n", (*ip)++, v);
		bp += strlen(bp);
	}
	else if (p->arity > 0)
	{
		bp = computekids(t->left, stringf("LEFT_CHILD(%s)", v), bp, ip);
		if (p->arity == 2)
			bp = computekids(t->right, stringf("RIGHT_CHILD(%s)", v), bp, ip);
	}
	return bp;
}

/* emitkids - emit burm_kids */
static void emitkids(Rule rules, int nrules)
{
	int i;
	Rule r, * rc = calloc(nrules+1, sizeof *rc);
	char** str = calloc(nrules+1, sizeof *str);

	for (i = 0, r = rules; r; r = r->link)
	{
		int j = 0;
		char buf[1024], * bp = buf;
		*computekids(r->pattern, "p", bp, &j) = 0;
		for (j = 0; str[j] && strcmp(str[j], buf); j++)
			;
		if (str[j] == NULL)
			str[j] = strdup(buf);
		r->kids = rc[j];
		rc[j] = r;
	}
	print("NODEPTR_TYPE *%Pkids(NODEPTR_TYPE p, int eruleno, NODEPTR_TYPE kids[]) {\n"
	      "%1%Passert(p, PANIC(\"NULL tree in %Pkids\\n\"));\n"
	      "%1%Passert(kids, PANIC(\"NULL kids in %Pkids\\n\"));\n"
	      "%1switch (eruleno) {\n");
	for (i = 0; r = rc[i]; i++)
	{
		for (; r; r = r->kids)
			print("%1case %d: /* %R */\n", r->ern, r);
		print("%s%2break;\n", str[i]);
	}
	print("%1default:\n%2%Passert(0, PANIC(\"Bad external rule number %%d in %Pkids\\n\", eruleno));\n%1}\n%1return kids;\n}\n\n");
}

/* emitlabel - emit the labelling functions */
static void emitlabel(Nonterm start)
{
	print("static void %Plabel1(NODEPTR_TYPE p) {\n"
	      "%1%Passert(p, PANIC(\"NULL tree in %Plabel\\n\"));\n"
	      "%1switch (%Parity[OP_LABEL(p)]) {\n"
	      "%1case 0:\n");
	if (Tflag)
		print("%2%Pnp = p;\n");
	print("%2STATE_LABEL(p) = %Pstate(p, 0, 0);\n%2break;\n"
	      "%1case 1:\n%2%Plabel1(LEFT_CHILD(p));\n");
	if (Tflag)
		print("%2%Pnp = p;\n");
	print("%2STATE_LABEL(p) = %Pstate(p,\n"
	      "%3STATE_LABEL(LEFT_CHILD(p)), 0);\n%2break;\n"
	      "%1case 2:\n%2%Plabel1(LEFT_CHILD(p));\n%2%Plabel1(RIGHT_CHILD(p));\n");
	if (Tflag)
		print("%2%Pnp = p;\n");
	print("%2STATE_LABEL(p) = %Pstate(p,\n"
	      "%3STATE_LABEL(LEFT_CHILD(p)),\n%3STATE_LABEL(RIGHT_CHILD(p)));\n%2break;\n"
	      "%1}\n}\n\n");
	print(
	    "STATE_TYPE %Plabel(NODEPTR_TYPE p) {\n%1%Plabel1(p);\n"
	    "%1return ((struct %Pstate *)STATE_LABEL(p))->rule.%P%S ? STATE_LABEL(p) : 0;\n"
	    "}\n\n",
	    start);
}

/* closure - fill in cost & rule with results of chain rules w/p as rhs */
static void closure(int cost[], Rule rule[], Nonterm p, int c)
{
	Rule r;

	for (r = p->chain; r; r = r->chain)
		if (c + r->cost < cost[r->lhs->number])
		{
			cost[r->lhs->number] = c + r->cost;
			rule[r->lhs->number] = r;
			closure(cost, rule, r->lhs, c + r->cost);
		}
}

/* computents - fill in bp with burm_nts vector for tree t */
static char* computents(Tree t, char* bp)
{
	if (t)
	{
		Nonterm p = t->op;
		if (p->kind == NONTERM)
		{
			sprintf(bp, "%s_%s_NT, ", prefix, p->name);
			bp += strlen(bp);
		}
		else
			bp = computents(t->right, computents(t->left, bp));
	}
	return bp;
}

/* emitnts - emit burm_nts ragged array */
static void emitnts(Rule rules, int nrules)
{
	Rule r;
	int i, j, * nts = calloc(nrules, sizeof *nts);
	char** str = calloc(nrules, sizeof *str);

	for (i = 0, r = rules; r; r = r->link)
	{
		char buf[1024];
		*computents(r->pattern, buf) = 0;
		for (j = 0; str[j] && strcmp(str[j], buf); j++)
			;
		if (str[j] == NULL)
		{
			print("static const short %Pnts_%d[] = { %s0 };\n", j, buf);
			str[j] = strdup(buf);
		}
		nts[i++] = j;
	}
	print("\nconst short *%Pnts[] = {\n");
	for (i = j = 0, r = rules; r; r = r->link)
	{
		for (; j < r->ern; j++)
			print("%10,%1/* %d */\n", j);
		print("%1%Pnts_%d,%1/* %d */\n", nts[i++], j++);
	}
	print("};\n\n");
}

/* emitrecord - emit code that tests for a winning match of rule r */
static void emitrecord(char* pre, Rule r, int cost)
{
	print("%sif (", pre);
	if (Tflag)
		print("%Ptrace(%Pnp, %d, c + %d, p->cost[%P%S_NT]), ",
		    r->ern, cost, r->lhs);
	print("c + %d < p->cost[%P%S_NT]) {\n"
	      "%s%1p->cost[%P%S_NT] = c + %d;\n%s%1p->rule.%P%S = %d;\n",
	    cost, r->lhs, pre, r->lhs, cost, pre, r->lhs,
	    r->packed);
	if (r->lhs->chain)
		print("%s%1%Pclosure_%S(p, c + %d);\n", pre, r->lhs, cost);
	print("%s}\n", pre);
}

/* emitrule - emit decoding vectors and burm_rule */
static void emitrule(Nonterm nts)
{
	Nonterm p;

	for (p = nts; p; p = p->link)
	{
		Rule r;
		print("static const short %Pdecode_%S[] = {\n%10,\n", p);
		for (r = p->rules; r; r = r->decode)
			print("%1%d,\n", r->ern);
		print("};\n\n");
	}
	print("int %Prule(STATE_TYPE state, int goalnt) {\n"
	      "%1%Passert(goalnt >= 1 && goalnt <= %d, PANIC(\"Bad goal nonterminal %%d in %Prule\\n\", goalnt));\n"
	      "%1if (!state)\n%2return 0;\n%1switch (goalnt) {\n",
	    ntnumber);
	for (p = nts; p; p = p->link)
		print("%1case %P%S_NT:"
		      "%1return %Pdecode_%S[((struct %Pstate *)state)->rule.%P%S];\n",
		    p, p, p);
	print("%1default:\n%2%Passert(0, PANIC(\"Bad goal nonterminal %%d in %Prule\\n\", goalnt));\n%1}\n%1return 0;\n}\n\n");
}

/* emitpredicates - emit predicates for rules */
static void emitpredicatedefinitions(Rule r)
{
	while (r)
	{
		struct stringfragment* f = r->when.first;
		if (f)
		{
			print("/* %R */\n", r);
			print("static int %Ppredicate_%d(NODEPTR_TYPE n) {\n", r->ern);
			while (f)
			{
				print("%s", f->data);
				f = f->next;
			}
			print("\n}\n\n");
		}
		r = r->link;
	}
}

static void print_path(uint32_t path)
{
	int i = 0;

	while (path > 0)
	{
		switch (path % 3)
		{
			case 1: print("LEFT_CHILD("); break;
			case 2: print("RIGHT_CHILD("); break;
		}
		path /= 3;
		i++;
	}

	print("node");

	while (i > 0)
	{
		print(")");
		i--;
	}
}

static const uint32_t PATH_MISSING = 0xffffffff;

static uint32_t find_label(Tree root, const char* name, uint32_t path)
{
	uint32_t p;

	if (root->label && (strcmp(root->label, name) == 0))
		return path;

	p = PATH_MISSING;
	if (root->left && (p == PATH_MISSING))
		p = find_label(root->left, name, path*3 + 1);
	if (root->right && (p == PATH_MISSING))
		p = find_label(root->right, name, path*3 + 2);
	return p;
}

static void label_not_found(Rule rule, const char* label)
{
	yylineno = rule->lineno;
	yyerror("label '%s' not found", label);
	exit(1);
}

/* emitinsndata - emit the code generation data */
static void emitinsndata(Rule rules)
{
	int k;
	Rule r;

	r = rules;
	while (r)
	{
		struct stringfragment* f = r->code.first;
		if (f)
		{
			print("/* %R */\n", r);
			print("static void %Pemitter_%d(NODEPTR_TYPE node, const struct %Pemitter_data* data) {\n", r->ern);

			while (f)
			{
				switch (f->data[0])
				{
					case '%':
					{
						const char* label = f->data + 1;

						if (strcmp(label, r->lhs->name) == 0)
							print("%1data->emit_resultreg();\n");
						else
						{
							uint32_t path = find_label(r->pattern, label, 0);
							print("%1data->emit_reg(");
							if (path == PATH_MISSING)
								label_not_found(r, label);
							print_path(path);
							print(");\n");
						}
						break;
					}

					case '$':
					{
						const char* label = f->data + 1;
						uint32_t path = find_label(r->pattern, label, 0);
						print("%1data->emit_value(");
						if (path == PATH_MISSING)
							label_not_found(r, label);
						print_path(path);
						print(");\n");
						break;
					}

					case '\n':
						assert(f->data[1] == 0);
						print("%1data->emit_eoi();\n");
						break;
					
					default:
						print("%1data->emit_string(\"%s\");\n", f->data);
				}

				f = f->next;
			}

			print("}\n\n");
		}

		r = r->link;
	}

	r = rules;
	print("const struct %Pinstruction_data %Pinstruction_data[] = {\n");
	k = 0;
	while (r)
	{
		for (; k < r->ern; k++)
			print("%1{ 0 }, /* %d */\n", k);
		k++;

		print("%1{ /* %d: %R */\n", r->ern, r);

		print("%2\"%R\",\n", r);

		print("%2");
		if (r->code.first)
			print("&%Pemitter_%d,\n", r->ern);
		else
			print("NULL,\n");

		if (r->lhs->allocate)
			print("%2%d,\n", r->lhs->allocate->number);
		else
			print("%20,\n");

		print("%2%s,\n", r->lhs->is_fragment ? "true" : "false");

		print("%1},\n");
		r = r->link;
	}
	print("};\n\n");
}

/* emitcost - emit a cost calculation via a predicate */
static void emitcostcalc(Rule r)
{
	if (r->when.first)
		print("!%Ppredicate_%d(node) ? %d : ", r->ern, maxcost);
}

/* emitstate - emit state function */
static void emitstate(Term terms, Nonterm start, int ntnumber)
{
	int i;
	Term p;

	print("STATE_TYPE %Pstate(NODEPTR_TYPE node, STATE_TYPE left, STATE_TYPE right) {\n%1int c;\n"
		  "%1int op = OP_LABEL(node);\n"
	      "%1struct %Pstate* p;\n"
		  "%1struct %Pstate* l = (struct %Pstate *)left;\n"
	      "%1struct %Pstate* r = (struct %Pstate *)right;\n"
		  "\n"
		  "%1assert(sizeof (STATE_TYPE) >= sizeof (void *));\n%1");
	print("%1p = malloc(sizeof *p);\n"
	      "%1p->op = op;\n"
		  "%1p->left = l;\n"
		  "%1p->right = r;\n"
		  "%1p->rule.%P%S = 0;\n",
	    start);
	for (i = 1; i <= ntnumber; i++)
		print("%1p->cost[%d] =\n", i);
	print("%2%d;\n"
		  "%1switch (op) {\n", maxcost);
	for (p = terms; p; p = p->link)
		emitcase(p, ntnumber);
	print("%1default:\n"
          "%2%Ppanic_cannot_match(node);\n"
		  "%1}\n"
	      "%1return (STATE_TYPE)p;\n}\n\n");
}

/* emitstring - emit array of rules and costs */
static void emitstring(Rule rules)
{
	Rule r;
	int k;

	print("static const short %Pcost[][4] = {\n");
	for (k = 0, r = rules; r; r = r->link)
	{
		for (; k < r->ern; k++)
			print("%1{ 0 },%1/* %d */\n", k);
		print("%1{ %d },%1/* %d = %R */\n", r->cost, k++, r);
	}
	print("};\n\n");
}

/* emitstruct - emit the definition of the state structure */
static void emitstruct(Nonterm nts, int ntnumber)
{
	print("struct %Pstate {\n%1int op;\n%1struct %Pstate *left, *right;\n"
	      "%1short cost[%d];\n%1struct {\n",
	    ntnumber + 1);
	for (; nts; nts = nts->link)
	{
		int n = 1, m = nts->lhscount;
		while (m >>= 1)
			n++;
		print("%2unsigned %P%S:%d;\n", nts, n);
	}
	print("%1} rule;\n};\n\n");
}

/* emitterms - emit terminal data structures */
static void emitterms(Term terms)
{
	Term p;
	int k;

	print("static const char %Parity[] = {\n");
	for (k = 0, p = terms; p; p = p->link)
	{
		for (; k < p->esn; k++)
			print("%10,%1/* %d */\n", k);
		print("%1%d,%1/* %d=%S */\n", p->arity < 0 ? 0 : p->arity, k++, p);
	}
	print("};\n\n");

	print("static const char *%Popname[] = {\n");
	for (k = 0, p = terms; p; p = p->link)
	{
		for (; k < p->esn; k++)
			print("%1/* %d */%10,\n", k);
		print("%1/* %d */%1\"%S\",\n", k++, p);
	}
	print("};\n\n");
}

/* emittest - emit clause for testing a match */
static void emittest(Tree t, char* v, char* suffix)
{
	Term p = t->op;

	if (p->kind == TERM)
	{
		print("%3%s->op == %d%s/* %S */\n", v, p->esn,
		    t->nterms > 1 ? " && " : suffix, p);
		if (t->left)
			emittest(t->left, stringf("%s->left", v),
			    t->right && t->right->nterms ? " && " : suffix);
		if (t->right)
			emittest(t->right, stringf("%s->right", v), suffix);
	}
}
