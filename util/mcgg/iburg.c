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
#include <inttypes.h>
#include "iburg.h"
#include "ircodes.h"
#include "astring.h"
#include "smap.h"
#include "mcgg.h"

#define REGATTR_INT 0
#define REGATTR_LONG 1
#define REGATTR_FLOAT 2
#define REGATTR_DOUBLE 3

int maxcost = SHRT_MAX / 2;

FILE* infp = NULL;
FILE* outfp = NULL;
FILE* hdrfp = NULL;

static char* prefix = "burm";
static int Tflag = 1; /* tracing */
static int ntnumber = 0;
static Nonterm start = 0;
static Term terms;
static Nonterm nts;
static Rule rules;
static int nrules;

static SMAPOF(struct reg) registers;
static SMAPOF(struct regattr) registerattrs;

static void print(const char* fmt, ...);
static void printh(const char* fmt, ...);
static void registerterminals(void);
static struct regattr* makeregattr(const char* id);
static void emitclosure(Nonterm nts);
static void emitcost(Tree t, const char* v);
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
static void emitregisterattrs();
static void emitregisterattrsptable();
static void emitregisterattrsqtable();
static void emitregisters();
static void emitrule(Nonterm nts);
static void emitstate(Term terms, Nonterm start, int ntnumber);
static void emitstring(Rule rules);
static void emitstruct(Nonterm nts, int ntnumber);
static void emitterms(Term terms);
static void emittest(Tree t, const char* v, const char* suffix);

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
    hdrfp = NULL;
    yy_flex_debug = 0;

    for (;;)
    {
        int opt = getopt(argc, argv, "p:i:o:h:yf");
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

            case 'h':
                hdrfp = fopen(optarg, "w");
                if (!hdrfp)
                {
                    yyerror(
                        "cannot open output header file: %s\n",
                        strerror(errno));
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

    /* Define some standard terms. */

    {
        const static struct terminfo reg = { "reg", NULL, "" };
        const static struct terminfo REG = { "REG", NULL, NULL };
        const static struct terminfo NOPI = { "NOP.I", NULL, NULL };
        const static struct terminfo NOPF = { "NOP.F", NULL, NULL };
        const static struct terminfo NOPL = { "NOP.L", NULL, NULL };
        const static struct terminfo NOPD = { "NOP.D", NULL, NULL };
        const static struct terminfo RET = { "RET", NULL, NULL };

        nonterm("reg", true);

        rule(NULL, tree(&reg, NULL, NULL))->cost = 1;
        rule(&reg, tree(&REG, NULL, NULL))->cost = 1;
        rule(&reg, tree(&NOPI, tree(&reg, NULL, NULL), NULL))->cost = 1;
        rule(&reg, tree(&NOPF, tree(&reg, NULL, NULL), NULL))->cost = 1;
        rule(&reg, tree(&NOPL, tree(&reg, NULL, NULL), NULL))->cost = 1;
        rule(&reg, tree(&NOPD, tree(&reg, NULL, NULL), NULL))->cost = 1;
        rule(NULL, tree(&RET, NULL, NULL))->cost = 1;
    }

    {
        struct regattr* attr = makeregattr("int");
        assert(attr->number == REGATTR_INT);
        attr = makeregattr("long");
        assert(attr->number == REGATTR_LONG);
        attr = makeregattr("float");
        assert(attr->number == REGATTR_FLOAT);
        attr = makeregattr("double");
        assert(attr->number == REGATTR_DOUBLE);
    }

    yyin = infp;
    yyparse();

    emitregisterattrs();
    emitregisterattrsptable();
    emitregisterattrsqtable();
    emitregisters();
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
    printh("#endif\n");

    if (outfp)
        fclose(outfp);
    if (hdrfp)
        fclose(hdrfp);

    return errcnt > 0;
}

static void
registerterminal(const struct ir_data* data, int iropcode, char type)
{
    const char* s
        = (type == 0) ? data->name : aprintf("%s.%c", data->name, type);
    int esn = ir_to_esn(iropcode, type);

    term(s, esn);
}

static void registerterminals(void)
{
    int i;

    for (i = 0; i < IR__COUNT; i++)
    {
        registerterminal(&ir_data[i], i, 'I');
        registerterminal(&ir_data[i], i, 'F');
        registerterminal(&ir_data[i], i, 'L');
        registerterminal(&ir_data[i], i, 'D');
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
        struct regattr rc;
    } sym;
    struct entry* link;
}* table[211];
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
    struct reg* p = smap_get(&registers, id);
    static int number = 0;

    if (p)
        yyerror("redefinition of '%s'", id);
    p = calloc(1, sizeof(*p));
    p->name = id;
    p->number = number++;
    array_append(&p->aliases, p);
    smap_put(&registers, id, p);

    return p;
}

void setregnames(struct reg* reg, struct stringlist* names)
{
    if (reg->names)
        yyerror("you can only set one set of register names");

    reg->names = names;
}

struct regattr* makeregattr(const char* id)
{
    struct regattr* p = smap_get(&registerattrs, id);
    static int number = 0;

    if (p)
        yyerror("redefinition of '%s'", id);
    p = calloc(1, sizeof(*p));
    p->name = id;
    p->number = number++;
    smap_put(&registerattrs, id, p);

    return p;
}

void addregattr(struct reg* reg, const char* id)
{
    struct regattr* p = smap_get(&registerattrs, id);

    if (!p)
        p = makeregattr(id);

    reg->attrs |= 1 << (p->number);
}

void addregalias(struct reg* r1, struct reg* r2)
{
    if (!array_appendu(&r1->aliases, r2))
    {
        int i;

        for (i = 0; i < r1->aliases.count; i++)
            addregalias(r1->aliases.item[i], r2);
    }
}

void addregaliases(struct reg* reg, struct stringlist* aliases)
{
    struct stringfragment* f = aliases->first;

    while (f)
    {
        struct reg* r = smap_get(&registers, f->data);
        if (!r)
            yyerror("register '%s' is not defined here", f->data);

        array_appendu(&reg->aliases, r);
        array_appendu(&r->aliases, reg);

        f = f->next;
    }
}

struct regattr* getregattr(const char* id)
{
    struct regattr* p = smap_get(&registerattrs, id);
    if (!p)
        yyerror("'%s' is not the name of a register class", id);
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
        yyerror("duplicate external symbol number `%s=%d'\n", p->name, p->esn);
    p->link = *q;
    *q = p;
    return p;
}

/* tree - create & initialize a tree node with the given fields */
Tree tree(const struct terminfo* ti, Tree left, Tree right)
{
    Tree t = calloc(1, sizeof *t);
    Term p = lookup(ti->name);
    int arity = 0;

    if (left && right)
        arity = 2;
    else if (left)
        arity = 1;
    if (p == NULL && arity > 0)
    {
        yyerror("undefined terminal `%s'\n", ti->name);
        p = term(ti->name, -1);
    }
    else if (p == NULL && arity == 0)
        p = (Term)nonterm(ti->name, false);
    else if (p && p->kind == NONTERM && arity > 0)
    {
        yyerror("`%s' is a non-terminal\n", ti->name);
        p = term(ti->name, -1);
    }
    if (p->kind == TERM && p->arity == -1)
        p->arity = arity;
    if (p->kind == TERM && arity != p->arity)
        yyerror("inconsistent arity for terminal `%s'\n", ti->name);
    t->op = p;
    t->nterms = p->kind == TERM;
    if (t->left = left)
        t->nterms += left->nterms;
    if (t->right = right)
        t->nterms += right->nterms;

    /* Special rules that have no output register attribute use "" as the
     * attribute name; these can't be made by the grammar. */

    t->label = ti->label;
    if ((p->kind == TERM) && (ti->attr))
        yyerror(
            "can't specify an input register attribute for terminal '%s'",
            ti->name);
    if (p->kind == NONTERM)
    {
        Nonterm nt = (Nonterm)p;
        if (nt->is_fragment && ti->attr)
            yyerror(
                "can't specify an input register attribute for fragment '%s'",
                ti->name);
        if (!nt->is_fragment && !ti->attr)
            yyerror(
                "must specify an input register attribute for non-fragment "
                "'%s'",
                ti->name);

        if (ti->attr && ti->attr[0])
        {
            t->attr = smap_get(&registerattrs, ti->attr);
            if (!t->attr)
                yyerror(
                    "'%s' doesn't seem to be a known register attribute",
                    ti->attr);
        }
    }
    return t;
}

/* rule - create & initialize a rule with the given fields */
Rule rule(const struct terminfo* ti, Tree pattern)
{
    static int number = 1;
    static const struct terminfo stmt = { "stmt", NULL, NULL };
    Rule r = calloc(1, sizeof *r);
    Rule* q;
    Term p = pattern->op;

    if (!ti)
        ti = &stmt;

    nrules++;
    r->lineno = yylineno;
    r->lhs = nonterm(ti->name, false);
    r->packed = ++r->lhs->lhscount;
    for (q = &r->lhs->rules; *q; q = &(*q)->decode)
        ;
    *q = r;
    r->pattern = pattern;
    r->ern = number++;
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

    r->label = ti->label;
    if (r->lhs->is_fragment && ti->attr)
        yyerror("can't specify an output register attribute for a fragment");
    if (!r->lhs->is_fragment && !ti->attr && (r->lhs->number != NONTERM_STMT))
        yyerror("must specify an output register attribute for non-fragments");

    /* Special rules that have no output register attribute use "" as the
     * attribute name; these can't be made by the grammar. */

    if (ti->attr && ti->attr[0])
    {
        r->attr = smap_get(&registerattrs, ti->attr);
        if (!r->attr)
            yyerror(
                "'%s' doesn't seem to be a known register attribute", ti->attr);
    }

    return r;
}

/* print - formatted output */

static void printto(FILE* fp, const char* fmt, va_list ap)
{
    if (!fp)
        return;

    for (; *fmt; fmt++)
        if (*fmt == '%')
            switch (*++fmt)
            {
                case 'd':
                    fprintf(fp, "%d", va_arg(ap, int));
                    break;

                case 'x':
                    fprintf(fp, "%x", va_arg(ap, uint32_t));
                    break;

                case 's':
                    fputs(va_arg(ap, char*), fp);
                    break;

                case 'P':
                    fprintf(fp, "%s_", prefix);
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
                    fputs(va_arg(ap, Term)->name, fp);
                    break;

                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                {
                    int n = *fmt - '0';
                    while (n-- > 0)
                        putc('\t', fp);
                    break;
                }

                default:
                    putc(*fmt, fp);
                    break;
            }
        else
            putc(*fmt, fp);
}

static void print(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printto(outfp, fmt, ap);
    va_end(ap);
}

static void printh(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printto(hdrfp, fmt, ap);
    va_end(ap);
}

static void emitregisterattrs(void)
{
    int i;

    printh("typedef uint_least");
    if (registerattrs.count <= 8)
        printh("8");
    else if (registerattrs.count <= 16)
        printh("16");
    else if (registerattrs.count <= 32)
        printh("32");
    else if (registerattrs.count <= 64)
        printh("64");
    else
        fatal("too many register classes");
    printh("_t regattr_t;\n");

    print("const char* %Pregister_class_names[] = {\n");
    for (i = 0; i < registerattrs.count; i++)
    {
        struct regattr* rc = registerattrs.item[i].right;
        assert(rc->number == i);

        print("%1\"%s\",\n", rc->name);
        if (rc->number > REGATTR_DOUBLE)
            printh("#define %P%s_ATTR (1U<<%d)\n", rc->name, rc->number);
    }
    print("};\n\n");
    printh("\n");
}

static void emitregisterattrsptable(void)
{
    int i, j;

    print("const uint8_t %Pregister_class_p_table[] = {\n");
    for (i = 0; i < registerattrs.count; i++)
    {
        struct regattr* rc = registerattrs.item[i].right;
        int count = 0;
        assert(rc->number == i);

        for (j = 0; j < registers.count; j++)
        {
            struct reg* r = registers.item[j].right;
            if (r->attrs & (1 << rc->number))
                count++;
        }

        print("%d, ", count);
    }
    print("\n};\n\n");
    printh("\n");
}

static void emitregisterattrsqtable(void)
{
    int i, j, k;

    print("const uint8_t %Pregister_class_q_table[] = {\n");
    for (i = 0; i < registerattrs.count; i++)
    {
        struct regattr* rc1 = registerattrs.item[i].right;
        uint64_t usage = 0;

        for (j = 0; j < registers.count; j++)
        {
            struct reg* r = registers.item[j].right;
            if (r->attrs & (1 << rc1->number))
            {
                for (k = 0; k < r->aliases.count; k++)
                    usage |= 1 << (r->aliases.item[k]->number);
            }
        }

        for (j = 0; j < registerattrs.count; j++)
        {
            int count = 0;
            struct regattr* rc2 = registerattrs.item[j].right;

            for (k = 0; k < registers.count; k++)
            {
                struct reg* r = registers.item[k].right;
                if (((r->attrs & (1 << rc2->number))
                     && (usage & (1 << r->number))))
                    count++;
            }

            print("%d, ", count);
        }

        print(" /* %s %x */\n", rc1->name, usage);
    }
    print("};\n\n");
    printh("\n");
}

static void emitregisters(void)
{
    int i, j;

    printh("typedef uint_least");
    if (registers.count <= 8)
        printh("8");
    else if (registers.count <= 16)
        printh("16");
    else if (registers.count <= 32)
        printh("32");
    else if (registers.count <= 64)
        printh("64");
    else
        fatal("too many registers");
    printh("_t regbits_t;\n");

    for (i = 0; i < registers.count; i++)
    {
        struct reg* r = registers.item[i].right;

        print(
            "const struct %Pregister_data* %Pregister_aliases_%d_%s[] = {\n%1",
            i, r->name);
        for (j = 0; j < r->aliases.count; j++)
            print("&%Pregister_data[%d], ", r->aliases.item[j]->number);
        print("NULL\n};\n");
    }

    for (i = 0; i < registers.count; i++)
    {
        struct reg* r = registers.item[i].right;

        print("const char* %Pregister_names_%d_%s[] = {\n%1", i, r->name);
        if (r->names)
        {
            struct stringfragment* f = r->names->first;
            while (f)
            {
                print("\"%s\", ", f->data);
                f = f->next;
            }
        }
        else
            print("\"%s\", ", r->name);
        print("NULL\n};\n");
    }

    print("const struct %Pregister_data %Pregister_data[] = {\n");
    for (i = 0; i < registers.count; i++)
    {
        struct reg* r = registers.item[i].right;
        uint32_t type = r->attrs & TYPE_ATTRS;
        assert(r->number == i);
        if (type & (type - 1))
            yyerror(
                "register %s has more than one type attribute set", r->name);

        print(
            "%1{ \"%s\", 0x%x, %Pregister_names_%d_%s, "
            "%Pregister_aliases_%d_%s },\n",
            r->name, r->attrs, i, r->name, i, r->name);
    }
    print("%1{ NULL }\n");
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
                    emittest(
                        r->pattern->left, "l",
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
        print("%d);\n", r->cost);
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
static void emitcost(Tree t, const char* v)
{
    Nonterm p = t->op;

    if (p->kind == TERM)
    {
        if (t->left)
            emitcost(t->left, aprintf("%s->left", v));
        if (t->right)
            emitcost(t->right, aprintf("%s->right", v));
    }
    else
        print("%s->cost[%P%S_NT] + ", v, p);
}

/* emitdefs - emit non-terminal defines and data structures */
static void emitdefs(Nonterm nts, int ntnumber)
{
    Nonterm p;

    printh("enum {\n");
    for (p = nts; p; p = p->link)
        printh("%1%P%S_NT = %d,\n", p, p->number);
    printh("%1%Pmax_nt = %d\n", ntnumber);
    printh("};\n\n");

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
          "%1%Passert(0, PANIC(\"Bad index %%d in %Pchild\\n\", "
          "index));\n%1return 0;\n}\n\n");
}

/* emitheader - emit initial definitions */
static void emitheader(void)
{
    print("#include \"mcgg_generated_header.h\"\n");
    if (Tflag)
        print("static NODEPTR_TYPE %Pnp;\n\n");

    printh("#ifndef MCG_DEFS_H\n");
    printh("#define MCG_DEFS_H\n\n");
}

void option(const char* o)
{
    printh("#define MCGG_OPTION_%s\n", o);
}

/* computekids - compute paths to kids in tree t */
static char* computekids(Tree node, const char* v, char* bp, int* ip)
{
    Term t = node->op;

    if (!node->left && !node->right)
    {
        sprintf(bp, "\t\tkids[%d] = %s;\n", (*ip)++, v);
        bp += strlen(bp);
    }

    if (t->kind == TERM)
    {
        if (t->arity >= 1)
            bp = computekids(node->left, aprintf("LEFT_CHILD(%s)", v), bp, ip);
        if (t->arity == 2)
            bp = computekids(
                node->right, aprintf("RIGHT_CHILD(%s)", v), bp, ip);
    }
    return bp;
}

/* emitkids - emit burm_kids */
static void emitkids(Rule rules, int nrules)
{
    int i;
    Rule r, *rc = calloc(nrules + 1, sizeof *rc);
    char** str = calloc(nrules + 1, sizeof *str);

    for (i = 0, r = rules; r; r = r->link)
    {
        int j = 0;
        char buf[1024], *bp = buf;
        *computekids(r->pattern, "p", bp, &j) = 0;
        for (j = 0; str[j] && strcmp(str[j], buf); j++)
            ;
        if (str[j] == NULL)
            str[j] = strdup(buf);
        r->kids = rc[j];
        rc[j] = r;
    }
    print("NODEPTR_TYPE *%Pkids(NODEPTR_TYPE p, int eruleno, NODEPTR_TYPE "
          "kids[]) {\n"
          "%1%Passert(p, PANIC(\"NULL tree in %Pkids\\n\"));\n"
          "%1%Passert(kids, PANIC(\"NULL kids in %Pkids\\n\"));\n"
          "%1switch (eruleno) {\n");
    for (i = 0; r = rc[i]; i++)
    {
        for (; r; r = r->kids)
            print("%1case %d: /* %R */\n", r->ern, r);
        print("%s%2break;\n", str[i]);
    }
    print("%1default:\n%2%Passert(0, PANIC(\"Bad external rule number %%d in "
          "%Pkids\\n\", eruleno));\n%1}\n%1return kids;\n}\n\n");
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
    print(
        "%2STATE_LABEL(p) = %Pstate(p,\n"
        "%3STATE_LABEL(LEFT_CHILD(p)), 0);\n%2break;\n"
        "%1case 2:\n%2%Plabel1(LEFT_CHILD(p));\n%2%Plabel1(RIGHT_CHILD(p));\n");
    if (Tflag)
        print("%2%Pnp = p;\n");
    print("%2STATE_LABEL(p) = %Pstate(p,\n"
          "%3STATE_LABEL(LEFT_CHILD(p)),\n%3STATE_LABEL(RIGHT_CHILD(p)));\n%"
          "2break;\n"
          "%1}\n}\n\n");
    print(
        "STATE_TYPE %Plabel(NODEPTR_TYPE p) {\n%1%Plabel1(p);\n"
        "%1return ((struct %Pstate *)STATE_LABEL(p))->rule.%P%S ? "
        "STATE_LABEL(p) : 0;\n"
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
        if (!t->left && !t->right)
        {
            if (p->kind == NONTERM)
                sprintf(bp, "%s_%s_NT, ", prefix, p->name);
            else
                sprintf(bp, "0, ");
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
    int i, j, *nts = calloc(nrules, sizeof *nts);
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
        print(
            "%Ptrace(%Pnp, %d, c + %d, p->cost[%P%S_NT]), ", r->ern, cost,
            r->lhs);
    print(
        "c + %d < p->cost[%P%S_NT]) {\n"
        "%s%1p->cost[%P%S_NT] = c + %d;\n%s%1p->rule.%P%S = %d;\n",
        cost, r->lhs, pre, r->lhs, cost, pre, r->lhs, r->packed);
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
    print(
        "int %Prule(STATE_TYPE state, int goalnt) {\n"
        "%1%Passert(goalnt >= 1 && goalnt <= %d, PANIC(\"Bad goal nonterminal "
        "%%d in %Prule\\n\", goalnt));\n"
        "%1if (!state)\n%2return 0;\n%1switch (goalnt) {\n",
        ntnumber);
    for (p = nts; p; p = p->link)
        print(
            "%1case %P%S_NT:"
            "%1return %Pdecode_%S[((struct %Pstate *)state)->rule.%P%S];\n",
            p, p, p);
    print("%1default:\n%2%Passert(0, PANIC(\"Bad goal nonterminal %%d in "
          "%Prule\\n\", goalnt));\n%1}\n%1return 0;\n}\n\n");
}

static void print_path(uint32_t path)
{
    int i = 0;

    while (path > 0)
    {
        switch (path % 3)
        {
            case 1:
                print("LEFT_CHILD(");
                break;
            case 2:
                print("RIGHT_CHILD(");
                break;
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

static uint32_t
find_label(Tree root, const char* name, uint32_t path, Tree* found)
{
    uint32_t p;

    if (root->label && (strcmp(root->label, name) == 0))
    {
        if (found)
            *found = root;
        return path;
    }

    p = PATH_MISSING;
    if (root->left && (p == PATH_MISSING))
        p = find_label(root->left, name, path * 3 + 1, found);
    if (root->right && (p == PATH_MISSING))
        p = find_label(root->right, name, path * 3 + 2, found);
    return p;
}

static void label_not_found(Rule rule, const char* label)
{
    yylineno = rule->lineno;
    yyerror("label '%s' not found", label);
    exit(1);
}

static bool
find_child_index(Tree node, const char* name, int* index, Tree* found)
{
    /* This must return the same ordering as the burm_kids() function uses. */

    if (node->label && strcmp(node->label, name) == 0)
    {
        if (found)
            *found = node;
        return true;
    }

    if (!node->left && !node->right)
        (*index)++;

    if (node->left && find_child_index(node->left, name, index, found))
        return true;
    if (node->right && find_child_index(node->right, name, index, found))
        return true;
    return false;
}

static void emit_predicate_expr(Rule r, struct expr* p)
{
    bool first = true;

    assert(p->type == PREDICATE_FUNCTION);
    print("%1if (%s%Ppredicate_%s(", p->negated ? "!" : "", p->u.name);

    p = p->next;
    while (p)
    {
        if (!first)
            print(", ");
        else
            first = false;

        switch (p->type)
        {
            case PREDICATE_NODE:
            {
                uint32_t path = find_label(r->pattern, p->u.name, 0, NULL);
                if (path == PATH_MISSING)
                    label_not_found(r, p->u.name);

                print_path(path);
                break;
            }

            case PREDICATE_NUMBER:
            {
                print("%d", p->u.number);
                break;
            }
        }

        p = p->next;
    }

    print("))");
}

/* emitpredicates - emit predicates for rules */
static void emitpredicatedefinitions(Rule r)
{
    int i;

    while (r)
    {
        print("/* %R */\n", r);
        print(
            "static int %Padjust_cost_%d(NODEPTR_TYPE node, int cost) {\n",
            r->ern);

        for (i = 0; i < r->prefers.count; i++)
        {
            emit_predicate_expr(r, r->prefers.item[i]);
            print(" cost -= 1;\n");
        }

        for (i = 0; i < r->requires.count; i++)
        {
            emit_predicate_expr(r, r->requires.item[i]);
            print(" {} else return %d;\n", maxcost);
        }

        print("%1if (cost > %d) return %d;\n", maxcost, maxcost);
        print("%1if (cost < 1) return 1;\n");
        print("%1return cost;\n");
        print("}\n\n");
        r = r->link;
    }
}

static void emit_input_regs(Tree node, int* index)
{
    /* This must return the same ordering as the burm_kids() function uses. */

    Nonterm nt = node->op;
    if ((nt->kind == NONTERM) && !nt->is_fragment && !node->left
        && !node->right)
    {
        if (node->attr)
        {
            uint32_t attr = 1 << node->attr->number;
            print(
                "%1data->constrain_input_reg(%d, 0x%x /* %s */);\n", *index,
                attr, node->attr->name);
        }
    }

    if (!node->left && !node->right)
        (*index)++;

    if (node->left)
        emit_input_regs(node->left, index);
    if (node->right)
        emit_input_regs(node->right, index);
}

static void emit_output_constraints(Rule r)
{
    int i;
    struct constraint* outputc = NULL;

    for (i = 0; i < r->constraints.count; i++)
    {
        struct constraint* c = r->constraints.item[i];

        if (c->type == CONSTRAINT_EQUALS)
        {
            if (strcmp(c->left, r->label) != 0)
                yyerror("equality register constraints must have an output "
                        "register on the left hand side");
            if (outputc != NULL)
                yyerror("you can't specify more than one output register "
                        "constraint");
            outputc = c;
        }
    }

    if (outputc)
    {
        int index = 0;
        Tree node;

        if (!find_child_index(r->pattern, outputc->right, &index, &node))
            label_not_found(r, outputc->right);
        if (r->attr != node->attr)
            yyerror("equality register constraints must have the same register "
                    "attribute on the left and right sides");

        print("%1data->constrain_output_reg_equal_to(%d);\n", index);
    }
}

static void emit_input_constraints(Rule r)
{
    int i;
    for (i = 0; i < r->constraints.count; i++)
    {
        int index;
        struct constraint* c = r->constraints.item[i];

        if (c->type == CONSTRAINT_PRESERVED)
        {
            if (strcmp(c->left, r->label) == 0)
                yyerror("cannot preserve an output register!");

            index = 0;
            if (!find_child_index(r->pattern, c->left, &index, NULL))
                label_not_found(r, c->left);

            print("%1data->constrain_input_reg_preserved(%d);\n", index);
        }
    }
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
        yylineno = r->lineno;

        if (!f)
        {
            /* This instruction has no code; make sure it's not a fragment. */
            if (r->lhs->is_fragment)
            {
                yylineno = r->lineno;
                yyerror("rule is a fragment, but doesn't emit anything");
            }
        }

        print("/* %R */\n", r);
        print(
            "static void %Pemitter_%d(const struct %Pemitter_data* data) {\n",
            r->ern);

        if (r->attr)
            print(
                "%1data->constrain_output_reg(0x%x /* %s */);\n",
                1 << r->attr->number, r->attr->name);

        {
            int index = 0;
            emit_input_regs(r->pattern, &index);
        }

        emit_output_constraints(r);
        emit_input_constraints(r);

        while (f)
        {
            char* data = strdup(f->data);
            int type = *data++;
            char* label = strtok(data, ".");
            char* nameindex_s = strtok(NULL, ".");
            int nameindex = nameindex_s ? atoi(nameindex_s) : 0;

            switch (type)
            {
                case '%':
                {
                    if (r->label && (strcmp(label, r->label) == 0))
                        print("%1data->emit_return_reg(%d);\n", nameindex);
                    else
                    {
                        Tree node;
                        int index = 0;
                        Nonterm nt;

                        if (!find_child_index(r->pattern, label, &index, &node))
                            label_not_found(r, label);
                        nt = node->op;
                        if (nt->kind == NONTERM)
                        {
                            if (nt->is_fragment)
                                print("%1data->emit_fragment(%d);\n", index);
                            else
                                print(
                                    "%1data->emit_reg(%d, %d);\n", index,
                                    nameindex);
                        }
                        else
                            print(
                                "%1data->emit_reg(%d, %d);\n", index,
                                nameindex);
                    }
                    break;
                }

                case '$':
                {
                    int index = 0;
                    if (!find_child_index(r->pattern, label, &index, NULL))
                        label_not_found(r, label);

                    if (nameindex != 0)
                        yyerror(
                            "indices other than 0 make no sense for $-values");

                    print("%1data->emit_value(%d);\n", index);
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

        print("%2&%Pemitter_%d,\n", r->ern);

        print("%2%s,\n", r->lhs->is_fragment ? "true" : "false");

        {
            int i;
            uint32_t attrs = 0;

            for (i = 0; i < r->constraints.count; i++)
            {
                struct constraint* c = r->constraints.item[i];

                if (c->type == CONSTRAINT_CORRUPTED_ATTR)
                {
                    struct regattr* p = smap_get(&registerattrs, c->left);
                    if (!p)
                        yyerror("no such register attribute '%s'", c->left);

                    attrs |= 1 << (p->number);
                }
            }

            print("%2%d, /* corruption attrs */\n", attrs);
        }

        print("%1},\n");
        r = r->link;
    }
    print("};\n\n");
}

/* emitcost - emit a cost calculation via a predicate */
static void emitcostcalc(Rule r)
{
    print("%Padjust_cost_%d(node, ", r->ern);
}

/* emitstate - emit state function */
static void emitstate(Term terms, Nonterm start, int ntnumber)
{
    int i;
    Term p;

    print("STATE_TYPE %Pstate(NODEPTR_TYPE node, STATE_TYPE left, STATE_TYPE "
          "right) {\n%1int c;\n"
          "%1int op = OP_LABEL(node);\n"
          "%1struct %Pstate* p;\n"
          "%1struct %Pstate* l = (struct %Pstate *)left;\n"
          "%1struct %Pstate* r = (struct %Pstate *)right;\n"
          "\n"
          "%1assert(sizeof (STATE_TYPE) >= sizeof (void *));\n%1");
    print(
        "%1p = malloc(sizeof *p);\n"
        "%1p->op = op;\n"
        "%1p->left = l;\n"
        "%1p->right = r;\n"
        "%1p->rule.%P%S = 0;\n",
        start);
    for (i = 1; i <= ntnumber; i++)
        print("%1p->cost[%d] =\n", i);
    print(
        "%2%d;\n"
        "%1switch (op) {\n",
        maxcost);
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
    print(
        "struct %Pstate {\n%1int op;\n%1struct %Pstate *left, *right;\n"
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
static void emittest(Tree t, const char* v, const char* suffix)
{
    Term p = t->op;

    if (p->kind == TERM)
    {
        print(
            "%3%s->op == %d%s/* %S */\n", v, p->esn,
            t->nterms > 1 ? " && " : suffix, p);
        if (t->left)
            emittest(
                t->left, aprintf("%s->left", v),
                t->right && t->right->nterms ? " && " : suffix);
        if (t->right)
            emittest(t->right, aprintf("%s->right", v), suffix);
    }
}

/* vim: set sw=4 ts=4 et: */

