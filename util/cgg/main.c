#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "em_spec.h"
#include "em_flag.h"
#include "em_reg.h"

#define EXTERN extern
#include "booth.h"
#undef EXTERN

/* Forward local declarations. */
static void debug(void);
static void verbose(void);
static void outregvar(void);
static void finishio(void);
static void inittables(void);
static void initio(void);
static int eqregclass(int r1, int r2);
static void compueq(void);

static int want_verbose = 0;


char *myalloc(int n)
{
	register char *p;

	p = malloc((unsigned) n);
	if (p == 0)
	{
		yyerror("Out of core");
		exit(1);
	}
	return (p);
}

void tstint(expr_t e)
{

	if (e.expr_typ != TYPINT)
		yyerror("Must be integer expression");
}

void tstbool(expr_t e)
{

	if (e.expr_typ != TYPBOOL)
		yyerror("Must be boolean expression");
}

int structsize(register list2 s)
{
	register list1 l;
	register int sum;

	sum = 0;
	while (s != 0)
	{
		l = s->l2list->l1next;
		while (l != 0)
		{
			sum++;
			l = l->l1next;
		}
		s = s->l2next;
	}
	return (sum);
}

static int isasc(int c)
{
	if ((c >= 0 && c <= 0177))
		return 1;
	return 0;
}

list2 lookstruct(list2 ll)
{
	list3 l3;
	list2 l21, l22;
	list1 l11, l12;

	for (l3 = structpool; l3 != 0; l3 = l3->l3next)
	{
		for (l21 = l3->l3list, l22 = ll; l21 != 0 && l22 != 0; l21 =
				l21->l2next, l22 = l22->l2next)
		{
			for (l11 = l21->l2list, l12 = l22->l2list;
					l11 != 0 && l12 != 0
							&& strcmp(l11->l1name, l12->l1name) == 0;
					l11 = l11->l1next, l12 = l12->l1next)
				;
			if (l11 != 0 || l12 != 0)
				goto contin;
		}
		if (l21 == 0 && l22 == 0)
			return (l3->l3list);
		contin: ;
	}
	l3 = (list3) myalloc(sizeof(struct list3str));
	l3->l3next = structpool;
	l3->l3list = ll;
	structpool = l3;
	return (ll);
}

int instno(inst_t inst)
{
	register int i, j;

	for (i = 1; i < narinstance; i++)
	{
		if (arinstance[i].in_which != inst.in_which)
			continue;
		for (j = 0; j < TOKENSIZE; j++)
			if (arinstance[i].in_info[j] != inst.in_info[j])
				goto cont;
		return (i);
		cont: ;
	}
	chktabsiz(narinstance, MAXINSTANCE, "Instance table");
	arinstance[narinstance] = inst;
	return (narinstance++);
}

string scopy(string s)
{
	register string t;

	t = (char *) myalloc(strlen(s) + 1);
	strcpy(t, s);
	return (t);
}

int strlookup(string s)
{
	register int i;

	for (i = 0; i < ncodestrings; i++)
		if (strcmp(s, codestrings[i]) == 0)
			return (i);
	chktabsiz(ncodestrings, MAXSTRINGS, "string table");
	codestrings[ncodestrings] = scopy(s);
	return (ncodestrings++);
}

int stringno(register string s)
{
	char buf[256];
	register char *p = buf;

	while (*s != 0)
		switch (*s)
		{
			default:
				*p++ = *s++;
				continue;
			case '$':
				s++;
				switch (*s)
				{
					default:
						yyerror("Bad character after $ in codestring");
					case '$':
						*p++ = *s++;
						continue;
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						*p++ = argtyp(patmnem[*s - '0']) == TYPINT ?
								PR_EMINT : PR_EMSTR;
						*p++ = *s++ - '0';
						continue;
				}
			case '%':
				s++;
				if (*s != '[')
				{
					if (*s == '%')
					{
						*p++ = *s++;
						continue;
					}
					else
						yyerror("Bad character following %% in codestring");
				}
				else
					s++;
				if (isdigit(*s))
				{
					int num;
					num = *s++ - '0';
					if (num < 1 || num > tokpatlen)
						yyerror("Number within %[] out of range");
					if (*s == ']')
					{
						s++;
						*p++ = PR_TOK;
						*p++ = num;
					}
					else if (*s++ != '.')
						yyerror(
								"Bad character following %%[digit in codestring");
					else
					{
						char field[256];
						register char *f = field;
						int type, offset;

						while (*s != ']' && *s != 0)
							*f++ = *s++;
						*f++ = 0;
						if (*s != ']')
							yyerror(
									"Unterminated %[] construction in codestring");
						else
							s++;
						if (isdigit(field[0]))
						{
							chkregexp(pattokexp[num]);
							*p++ = PR_SUBREG;
							*p++ = num;
							*p++ = atoi(field);
						}
						else
						{
							offset = findstructel(pattokexp[num], field, &type);
							*p++ = PR_TOKFLD;
							*p++ = num;
							*p++ = offset;
						}
					}
				}
				else if (*s >= 'a' && *s < 'a' + nallreg)
				{
					int reg, subreg;
					reg = *s++ - 'a' + 1;
					if (*s == ']')
						subreg = 255;
					else
					{
						if (*s != '.')
							yyerror(
									"Bad character following %%[x in codestring");
						s++;
						if (!isdigit(*s))
							yyerror(
									"Bad character following %%[x. in codestring");
						subreg = *s - '0';
						s++;
						if (*s != ']')
							yyerror(
									"Bad character following %%[x.y in codestring");
					}
					s++;
					*p++ = PR_ALLREG;
					*p++ = reg;
					*p++ = subreg;
				}
				else
					yyerror("Bad character following %%[ in codestring");
		}
	*p++ = 0;
	return (strlookup(buf));
}

void tabovf(string tablename)
{
	char buf[256];

	sprintf(buf, "%s overflow", tablename);
	yyerror(buf);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{

	while (--argc)
	{
		++argv;
		if (argv[0][0] == '-')
		{
			switch (argv[0][1])
			{
				case 'h':
					hname = &argv[0][2];
					break;
				case 'c':
					cname = &argv[0][2];
					break;
				case 'v':
					want_verbose = 1;
					break;
				default:
					fprintf(stderr, "Bad flag %s\n", argv[0]);
					break;
			}
		}
		else
		{
			iname = argv[0];
		}
	}
	inithash();
	initio();
	inittables();
	yyparse();
	if (nerrors == 0)
	{
		compueq();
		hashpatterns();
		finishio();
		if (want_verbose)
			verbose();
	}
	if (want_verbose)
		debug();
	exit(nerrors);
}

int lookup(int comm, int operator, int lnode, int rnode)
{
	register node_p p;

	for (p = nodes + 1; p < lastnode; p++)
	{
		if (p->ex_operator != operator)
			continue;
		if (!((p->ex_lnode == lnode && p->ex_rnode == rnode)
				|| (comm && p->ex_lnode == rnode && p->ex_rnode == lnode)))
			continue;
		return (p - nodes);
	}
	if (lastnode >= &nodes[MAXNODES])
		yyerror("node table overflow");
	lastnode++;
	p->ex_operator = operator;
	p->ex_lnode = lnode;
	p->ex_rnode = rnode;
	return (p - nodes);
}

static void compueq(void)
{
	register int i, j;

	for (i = 1; i < nmachregs; i++)
	{
		for (j = 1; j < i; j++)
			if (eqregclass(i, j))
			{
				stregclass[i] = stregclass[j];
				break;
			}
		if (j == i)
			stregclass[i] = nregclasses++;
	}
}

static int eqregclass(int r1, int r2)
{
	register reginfo rp1, rp2;
	register int i;
	short regbits[(MAXREGS + 15) >> 4];
	int member;

	rp1 = machregs[r1];
	rp2 = machregs[r2];
	for (i = 0; i < ((nprops + 15) >> 4); i++)
		if (rp1->rprop[i] != rp2->rprop[i])
			return (0);
	for (i = 0; i < ((MAXREGS + 15) >> 4); i++)
		regbits[i] = 0;
	for (i = 0; i < maxmembers; i++)
	{
		if ((member = rp1->rmembers[i]))
			regbits[member >> 4] |= (1 << (member & 017));
	}
	for (i = 0; i < maxmembers; i++)
	{
		member = rp2->rmembers[i];
		if (regbits[member >> 4] & (1 << (member & 017)))
			return (0);
	}
	return (1);
}

unsigned hash(register string name)
{
	register unsigned sum;
	register int i;

	for (sum = i = 0; *name; i += 3)
		sum ^= (*name++) << (i & 07);
	return (sum);
}

ident_p ilookup(string name, int enterf)
{
	register ident_p p, *pp;

	pp = &identtab[hash(name) % ITABSIZE];
	while (*pp != 0)
	{
		if (strcmp((*pp)->i_name, name) == 0)
		{
			if (enterf != ENTER)
				return (*pp);
			else
				yyerror("Multiply defined symbol");
		}
		pp = &(*pp)->i_next;
	}
	if (enterf == LOOKUP)
		yyerror("Undefined symbol");
	if (enterf == JUSTLOOKING)
		return (0);
	p = *pp = (ident_p) myalloc(sizeof(ident_t));
	p->i_name = name;
	p->i_next = 0;
	p->i_type = 0;
	return (p);
}

static void initio(void)
{

	if (iname != 0 && freopen(iname, "r", stdin) == NULL)
	{
		fprintf(stderr, "Can't open %s\n", iname);
		exit(-1);
	}
	if ((cfile = fopen(cname, "w")) == NULL)
	{
		fprintf(stderr, "Can't create %s\n", cname);
		exit(-1);
	}
	if ((hfile = fopen(hname, "w")) == NULL)
	{
		fprintf(stderr, "Can't create %s\n", hname);
		exit(-1);
	}
	fprintf(cfile, "#include \"param.h\"\n");
	fprintf(cfile, "#include \"tables.h\"\n");
	fprintf(cfile, "#include \"types.h\"\n");
	fprintf(cfile, "#include <cg_pattern.h>\n");
	fprintf(cfile, "#include \"data.h\"\n");
	fprintf(cfile, "\nbyte coderules[] = {\n");
	patbyte(0);
}

int exprlookup(set_t sett)
{
	register int i, j, ok;

	for (i = 0; i < nmachsets; i++)
	{
		ok = (sett.set_size == machsets[i].set_size);
		for (j = 0; j < SETSIZE; j++)
		{
			if (sett.set_val[j] == machsets[i].set_val[j])
				continue;
			ok = 0;
			break;
		}
		if (ok)
			return (i);
	}
	chktabsiz(nmachsets, MAXSETS, "Expression table");
	machsets[nmachsets] = sett;
	return (nmachsets++);
}

static void inittables(void)
{
	register reginfo r;
	register int i;
	inst_t inst;
	set_t sett;

	nodes[0].ex_operator = EX_CON;
	nodes[0].ex_lnode = 0;
	nodes[0].ex_rnode = 0;
	cocopropno = nprops++;
	r = (reginfo) myalloc(sizeof(struct reginfo));
	r->rname = "cc reg";
	r->rrepr = "CC";
	r->rsize = -1;
	r->rregvar = -1;
	for (i = 0; i < MAXMEMBERS; i++)
		r->rmembers[i] = 0;
	for (i = 0; i < PROPSETSIZE; i++)
		r->rprop[i] = 0;
	r->rprop[cocopropno >> 4] |= (1 << (cocopropno & 017));
	chktabsiz(nmachregs, MAXREGS, "Register table");
	machregs[nmachregs++] = r;
	inst.in_which = IN_RIDENT;
	inst.in_info[0] = nmachregs - 1;
	for (i = 1; i < TOKENSIZE; i++)
		inst.in_info[i] = 0;
	ccinstanceno = instno(inst);
	ccregexpr = lookup(0, EX_REG, nmachregs - 1, 0);
	sett.set_size = 0;
	for (i = 0; i < SETSIZE; i++)
		sett.set_val[i] = 0;
	sett.set_val[nmachregs >> 4] |= (01 << (nmachregs & 017));
	cocosetno = exprlookup(sett);
}

static void outregs(void)
{
	register int i, j, k;
	static short rset[(MAXREGS + 15) >> 4];
	int t, ready;

	fprintf(cfile, "char stregclass[] = {\n");
	for (i = 0; i < nmachregs; i++)
		fprintf(cfile, "\t%d,\n", stregclass[i]);
	fprintf(cfile, "};\n\nstruct reginfo machregs[] = {\n{0},\n");
	for (i = 1; i < nmachregs; i++)
	{
		fprintf(cfile, "{%d,%d", strlookup(machregs[i]->rrepr),
				machregs[i]->rsize);
		if (maxmembers != 0)
		{
			fprintf(cfile, ",{");
			for (j = 0; j < maxmembers; j++)
				fprintf(cfile, "%d,", machregs[i]->rmembers[j]);
			/* now compute and print set of registers
			 * that clashes with this register.
			 * A register clashes with al its children (and theirs)
			 * and with all their parents.
			 */
			for (j = 0; j < ((MAXREGS + 15) >> 4); j++)
				rset[j] = 0;
			rset[i >> 4] |= (1 << (i & 017));
			do
			{
				ready = 1;
				for (j = 1; j < nmachregs; j++)
					if (rset[j >> 4] & (1 << (j & 017)))
						for (k = 0; k < maxmembers; k++)
							if ((t = machregs[j]->rmembers[k]) != 0)
							{
								if ((rset[t >> 4] & (1 << (t & 017))) == 0)
									ready = 0;
								rset[t >> 4] |= (1 << (t & 017));
							}
			} while (!ready);
			do
			{
				ready = 1;
				for (j = 1; j < nmachregs; j++)
					for (k = 0; k < maxmembers; k++)
						if ((t = machregs[j]->rmembers[k]) != 0)
							if (rset[t >> 4] & (1 << (t & 017)))
							{
								if ((rset[j >> 4] & (1 << (j & 017))) == 0)
									ready = 0;
								rset[j >> 4] |= (1 << (j & 017));
							}
			} while (!ready);
			fprintf(cfile, "},{");
			for (j = 0; j < ((nmachregs + 15) >> 4); j++)
				fprintf(cfile, "%d,", rset[j]);
			fprintf(cfile, "}");
		}
		if (machregs[i]->rregvar >= 0)
			fprintf(cfile, ",1");
		fprintf(cfile, "},\n");
	}
	fprintf(cfile, "};\n\n");
}

static void finishio(void)
{
	register int i;
	register node_p np;
	int j;
	int setsize;
	register move_p mp;

	fprintf(cfile, "};\n\n");
	if (wsize > 0)
		fprintf(hfile, "#define TEM_WSIZE %d\n", wsize);
	else
		yyerror("Wordsize undefined");
	if (psize > 0)
		fprintf(hfile, "#define TEM_PSIZE %d\n", psize);
	else
		yyerror("Pointersize undefined");
	if (bsize >= 0)
		fprintf(hfile, "#define TEM_BSIZE %d\n", bsize);
	else
		yyerror("EM_BSIZE undefined");
	if (fmt != 0)
		fprintf(hfile, "#define WRD_FMT \"%s\"\n", fmt);
	fprintf(hfile, "#define MAXALLREG %d\n", maxallreg);
	setsize = (nmachregs + 1 + nmachtokens + 15) >> 4;
	fprintf(hfile, "#define SETSIZE %d\n", setsize);
	fprintf(hfile, "#define NPROPS %d\n", nprops);
	fprintf(hfile, "#define NREGS %d\n", nmachregs);
	fprintf(hfile, "#define REGSETSIZE %d\n", (nmachregs + 15) >> 4);
	fprintf(hfile, "#define TOKENSIZE %d\n", maxtokensize);
	fprintf(hfile, "#define MAXMEMBERS %d\n", maxmembers);
	fprintf(hfile, "#define LONGESTPATTERN %d\n", maxempatlen);
	fprintf(hfile, "#define MAXRULE %d\n", maxrule);
	fprintf(hfile, "#define NMOVES %d\n", nmoves);
	fprintf(hfile, "#define NC1 %d\n", nc1);
	if (nc2)
	{
		assert(maxsplit != 0);
		fprintf(hfile, "#define NC2 %d\n", nc2);
		fprintf(hfile, "#define MAXSPLIT %d\n", maxsplit);
	}
	fprintf(hfile, "#define NC3 %d\n", nc3);
	outregs();
	fprintf(cfile, "tkdef_t tokens[] = {\n");
	for (i = 0; i < nmachtokens; i++)
	{
		fprintf(cfile, "{%d,{%d,%d},{", machtokens[i].t_size,
				machtokens[i].t_cost.c_size, machtokens[i].t_cost.c_time);
		for (j = 0; j < maxtokensize; j++)
			fprintf(cfile, "%d,", machtokens[i].t_fields[j].t_type);
		fprintf(cfile, "},%d},\n", machtokens[i].t_format);
	}
	fprintf(cfile, "};\n\nnode_t enodes[] = {\n");
	for (np = nodes; np < lastnode; np++)
		fprintf(cfile, "{%d,%d,%d},\n", np->ex_operator, np->ex_lnode,
				np->ex_rnode);
	fprintf(cfile, "};\n\nstring codestrings[] = {\n");
	for (i = 0; i < ncodestrings; i++)
	{
		register char *p;
		p = codestrings[i];
		fprintf(cfile, "\t\"");
		while (*p)
		{
			register int c = (*p) & BMASK;
			if (!isasc(c) || iscntrl(c))
			{
				/* The next line used to have (c>>6)&03,
				 but this triggered a bug in GCC 2.4.5
				 on SPARC.
				 */
				fprintf(cfile, "\\%c%c%c", ((*p >> 6) & 03) + '0',
						((c >> 3) & 07) + '0', (c & 07) + '0');
			}
			else
				putc(c, cfile);
			p++;
		}
		fprintf(cfile, "\",\n");
	}
	fprintf(cfile, "};\n\nset_t machsets[] = {\n");
	for (i = 0; i < nmachsets; i++)
	{
		fprintf(cfile, "{%d,{", machsets[i].set_size);
		for (j = 0; j < setsize; j++)
			fprintf(cfile, "0%o,", machsets[i].set_val[j] & 0xFFFF);
		fprintf(cfile, "}},\n");
	}
	fprintf(cfile, "};\n\ninst_t tokeninstances[] = {\n");
	for (i = 0; i < narinstance; i++)
	{
		fprintf(cfile, "{ %d, {", arinstance[i].in_which);
		for (j = 0; j <= maxtokensize; j++)
			fprintf(cfile, "%d,", arinstance[i].in_info[j]);
		fprintf(cfile, "}},\n");
	}
	fprintf(cfile, "};\n\nmove_t moves[] = {\n");
	for (i = 0; i < nmoves; i++)
	{
		mp = &machmoves[i];
		fprintf(cfile, "{%d,%d,%d,%d,%d,{%d,%d}},\n", mp->m_set1, mp->m_expr1,
				mp->m_set2, mp->m_expr2, mp->m_cindex, mp->m_cost.c_size,
				mp->m_cost.c_time);
	}
	fprintf(cfile, "};\n\nbyte pattern[] = {\n");
	for (i = 0; i < npatbytes; i++)
	{
		fprintf(cfile, "%3d,", pattern[i] & BMASK);
		if ((i % 10) == 9)
			fprintf(cfile, "\n");
	}
	fprintf(cfile, "\n};\n\nint pathash[256] = {\n");
	for (i = 0; i < 256; i++)
	{
		fprintf(cfile, "%6d,", pathash[i]);
		if ((i & 07) == 07)
			fprintf(cfile, "\n");
	}
	fprintf(cfile, "};\n\nc1_t c1coercs[] = {\n");
	for (i = 0; i < nc1; i++)
		fprintf(cfile, "{%d,%d,%d,%d,{%d,%d}},\n", c1coercs[i].c1_texpno,
				c1coercs[i].c1_expr, c1coercs[i].c1_prop, c1coercs[i].c1_codep,
				c1coercs[i].c1_cost.c_size, c1coercs[i].c1_cost.c_time);
	if (nc2)
		fprintf(cfile, "};\n\nc2_t c2coercs[] = {\n");
	for (i = 0; i < nc2; i++)
	{
		fprintf(cfile, "{%d,%d,{", c2coercs[i].c2_texpno,
				c2coercs[i].c2_nsplit);
		for (j = 0; j < maxsplit; j++)
			fprintf(cfile, "%d,", c2coercs[i].c2_repl[j]);
		fprintf(cfile, "},%d},\n", c2coercs[i].c2_codep);
	}
	fprintf(cfile, "};\n\nc3_t c3coercs[] = {\n");
	for (i = 0; i < nc3; i++)
		fprintf(cfile, "{%d,%d,%d,%d},\n", c3coercs[i].c3_texpno,
				c3coercs[i].c3_prop, c3coercs[i].c3_repl, c3coercs[i].c3_codep);
	fprintf(cfile, "};\n\n");
	for (i = 0; i < nprops; i++)
	{
		fprintf(cfile, "struct reginfo *rlist%d[] = {\n", i);
		for (j = 2; j <= nmachregs; j++)
		{
			if (machregs[j - 1]->rregvar < 0
					&& (machprops[i].propset.set_val[j >> 4] & (1 << (j & 017))))
				fprintf(cfile, "\t&machregs[%d],\n", j - 1);
		}
		fprintf(cfile, "\t0\n};\n");
	}
	fprintf(cfile, "struct reginfo **reglist[] = {\n");
	for (i = 0; i < nprops; i++)
	{
		fprintf(cfile, "\trlist%d,\n", i);
	}
	fprintf(cfile, "};\n");
	fprintf(cfile, "unsigned cc1 = %u;\n", cc1);
	fprintf(cfile, "unsigned cc2 = %u;\n", cc2);
	fprintf(cfile, "unsigned cc3 = %u;\n", cc3);
	fprintf(cfile, "unsigned cc4 = %u;\n", cc4);
	if (rvused)
		outregvar();
}

static void outregvar(void)
{
	register int i, j;

	fprintf(hfile, "#define REGVARS\n");
	fprintf(cfile, "#include \"regvar.h\"\n");
	fprintf(cfile, "int nregvar[4] = { ");
	for (i = 0; i < 4; i++)
		fprintf(cfile, "%d, ", nregvar[i]);
	fprintf(cfile, "};\n");
	for (i = 0; i < 4; i++)
		if (nregvar[i] > 0)
			fprintf(cfile, "struct regassigned ratar%d[%d];\n", i, nregvar[i]);
	for (i = 0; i < 4; i++)
		if (nregvar[i] > 0)
		{
			fprintf(cfile, "int rvtar%d[] = {", i);
			for (j = 0; j < nregvar[i]; j++)
				fprintf(cfile, "%d,", rvnumbers[i][j]);
			fprintf(cfile, "};\n");
		}
	fprintf(cfile, "\nint *rvnumbers[] = {\n");
	for (i = 0; i < 4; i++)
		if (nregvar[i] > 0)
			fprintf(cfile, "\trvtar%d,\n", i);
		else
			fprintf(cfile, "\t0,\n");
	fprintf(cfile, "};\n\nstruct regassigned *regassigned[] = {\n");
	for (i = 0; i < 4; i++)
		if (nregvar[i] > 0)
			fprintf(cfile, "\tratar%d,\n", i);
		else
			fprintf(cfile, "\t0,\n");
	fprintf(cfile, "};\n");
}

static void verbose(void)
{
	fprintf(stderr, "Codebytes %d\n", codebytes);
	fprintf(stderr, "Registers %d(%d)\n", nmachregs, MAXREGS);
	fprintf(stderr, "Properties %d(%d)\n", nprops, MAXPROPS);
	fprintf(stderr, "Tokens %d(%d)\n", nmachtokens, MAXTOKENS);
	fprintf(stderr, "Sets %d(%d)\n", nmachsets, MAXSETS);
	fprintf(stderr, "Tokeninstances %d(%d)\n", narinstance, MAXINSTANCE);
	fprintf(stderr, "Strings %d(%d)\n", ncodestrings, MAXSTRINGS);
	fprintf(stderr, "Enodes %d(%d)\n", (int)(lastnode - nodes), MAXNODES);
	fprintf(stderr, "Patbytes %d(%d)\n", npatbytes, MAXPATTERN);
}

void inbetween(void)
{
	register ident_p ip;
	register int i, j;
	register move_p mp;

	lookident = 1; /* for lexical analysis */

	chktabsiz(nmachsets + 1, MAXSETS, "Expressiontable");
	for (i = 0; i < SETSIZE; i++)
		machsets[nmachsets].set_val[i] = (short)0xFFFF;
	machsets[nmachsets].set_val[0] &= ~1;
	machsets[nmachsets].set_size = 0;
	ip = ilookup("SCRATCH", ENTER);
	ip->i_type = IEXP;
	ip->i_i.i_expno = nmachsets++;

	for (i = 0; i < SETSIZE; i++)
		machsets[nmachsets].set_val[i] = (short)0xFFFF;
	machsets[nmachsets].set_size = 0;
	ip = ilookup("ALL", ENTER);
	ip->i_type = IEXP;
	allexpno = ip->i_i.i_expno = nmachsets++;
	mp = &machmoves[nmoves++];
	mp->m_set1 = cocosetno;
	mp->m_expr1 = 0;
	mp->m_set2 = nmachsets - 1;
	mp->m_expr2 = 0;
	mp->m_cindex = 0;
	mp->m_cost.c_size = 0;
	mp->m_cost.c_time = 0;

	/*
	 * Create sets of registers per property
	 */

	for (i = 0; i < nprops; i++)
	{
		short *sp = machprops[i].propset.set_val;

		sp[0] |= 1;
		for (j = 2; j <= nmachregs; j++)
			if (machregs[j - 1]->rprop[i >> 4] & (1 << (i & 017)))
				sp[j >> 4] |= (1 << (j & 017));
	}
}

int formconversion(register char *p, register token_p tp)
{
	char buf[256];
	register char *q = buf;
	char field[256];
	register char *f;
	int i;

	if (p == 0)
		return (0);
	while (*p)
		switch (*p)
		{
			default:
				*q++ = *p++;
				continue;
			case '%':
				p++;
				if (*p == '%')
				{
					*q++ = *p++;
					continue;
				}
				if (*p == '[')
					p++;
				else
					yyerror("Bad character after % in format");
				f = field;
				while (*p != 0 && *p != ']')
					*f++ = *p++;
				*f++ = 0;
				if (*p == ']')
					p++;
				else
					yyerror("Unterminated %[] construct in format");
				for (i = 0; i < TOKENSIZE - 1; i++)
					if (strcmp(field, tp->t_fields[i].t_sname) == 0)
						break;
				if (i == TOKENSIZE - 1)
					yyerror("Unknown field in %[] construct in format");
				*q++ = i + 1;
		}
	*q++ = 0;
	return (strlookup(buf));
}

void setfields(register token_p tp, string format)
{
	register int i;
	list2 ll;
	register list1 l;
	int type;

	for (i = 0; i < TOKENSIZE - 1; i++)
		tp->t_fields[i].t_type = 0;
	i = 0;
	for (ll = tp->t_struct; ll != 0; ll = ll->l2next)
	{
		l = ll->l2list;
		if (strcmp(l->l1name, "REGISTER") == 0)
			type = TYPREG;
		else if (strcmp(l->l1name, "INT") == 0)
			type = TYPINT;
		else
			type = TYPSTR;
		for (l = l->l1next; l != 0; l = l->l1next)
		{
			tp->t_fields[i].t_type = type;
			tp->t_fields[i].t_sname = l->l1name;
			i++;
		}
	}
	if (format != 0)
		tp->t_format = formconversion(format, tp);
	else
		tp->t_format = -1;
}

void chkregexp(int number)
{
	register int i;

	for (i = nmachregs + 1; i < nmachregs + 1 + nmachtokens; i++)
		if (machsets[number].set_val[i >> 4] & (01 << (i & 017)))
			yyerror("No tokens allowed in this set");
}

int findstructel(int number, string name, int *t)
{
	register int i;
	register token_p tp;
	register list2 structdecl;
	int offset;

	for (i = 1; i <= nmachregs; i++)
		if (machsets[number].set_val[i >> 4] & (01 << (i & 017)))
			yyerror("No registers allowed in this set");
	structdecl = 0;
	for (i = nmachregs + 1; i < nmachregs + 1 + nmachtokens; i++)
	{
		if (machsets[number].set_val[i >> 4] & (01 << (i & 017)))
		{
			if (structdecl == 0)
			{
				structdecl = machtokens[i - (nmachregs + 1)].t_struct;
				tp = &machtokens[i - (nmachregs + 1)];
			}
			else if (structdecl != machtokens[i - (nmachregs + 1)].t_struct)
				yyerror("Multiple structs in this set");
		}
	}
	if (structdecl == 0)
	{
		yyerror("No structs in this set");
		return (0);
	}
	for (offset = 0; offset < TOKENSIZE - 1; offset++)
		if (tp->t_fields[offset].t_type != 0
				&& strcmp(tp->t_fields[offset].t_sname, name) == 0)
		{
			*t = tp->t_fields[offset].t_type;
			return (offset + 1);
		}
	yyerror("No such field in this struct");
	return (0);
}

extern char em_flag[];

int argtyp(int mn)
{

	switch (em_flag[mn - sp_fmnem] & EM_PAR)
	{
		case PAR_W:
		case PAR_S:
		case PAR_Z:
		case PAR_O:
		case PAR_N:
		case PAR_L:
		case PAR_F:
		case PAR_R:
		case PAR_C:
			return (TYPINT);
		default:
			return (TYPSTR);
	}
}

int commontype(expr_t e1, expr_t e2)
{
	if (e1.expr_typ != e2.expr_typ)
		yyerror("Type incompatibility");
	return (e1.expr_typ);
}

extern char em_mnem[][4];

#define HASHSIZE        (2*(sp_lmnem-sp_fmnem))

struct hashmnem
{
	char h_name[3];
	byte h_value;
} hashmnem[HASHSIZE];

void inithash(void)
{
	register int i;

	for (i = 0; i <= sp_lmnem - sp_fmnem; i++)
		enter(em_mnem[i], i + sp_fmnem);
}

void enter(char *name, int value)
{
	register unsigned h;

	h = hash(name) % HASHSIZE;
	while (hashmnem[h].h_name[0] != 0)
		h = (h + 1) % HASHSIZE;
	strncpy(hashmnem[h].h_name, name, 3);
	hashmnem[h].h_value = value;
}

int mlookup(char *name)
{
	register unsigned int h;

	h = hash(name) % HASHSIZE;
	while (strncmp(hashmnem[h].h_name, name, 3) != 0
			&& hashmnem[h].h_name[0] != 0)
		h = (h + 1) % HASHSIZE;
	return (hashmnem[h].h_value & BMASK); /* 0 if not found */
}

void hashpatterns(void)
{
	short index;
	register byte *bp, *tp;
	register short i;
	unsigned short hashvalue;
	int patlen;

	index = prevind;
	while (index != 0)
	{
		bp = &pattern[index];
		tp = &bp[PO_MATCH];
		i = *tp++ & BMASK;
		if (i == BMASK)
		{
			i = *tp++ & BMASK;
			i |= (*tp++ & BMASK) << BSHIFT;
		}
		patlen = i;
		hashvalue = 0;
		switch (patlen)
		{
			default: /* 3 or more */
				hashvalue = (hashvalue << 4) ^ (*tp++ & BMASK);
			case 2:
				hashvalue = (hashvalue << 4) ^ (*tp++ & BMASK);
			case 1:
				hashvalue = (hashvalue << 4) ^ (*tp++ & BMASK);
		}
		assert(hashvalue!= ILLHASH);
		i = index;
		index = (bp[PO_NEXT] & BMASK) | (bp[PO_NEXT + 1] << BSHIFT);
		bp[PO_HASH] = hashvalue >> BSHIFT;
		hashvalue &= BMASK;
		bp[PO_NEXT] = pathash[hashvalue] & BMASK;
		bp[PO_NEXT + 1] = pathash[hashvalue] >> BSHIFT;
		pathash[hashvalue] = i;
	}
}

static void debug(void)
{
	register int i, j;

	for (i = 0; i < ITABSIZE; i++)
	{
		register ident_p ip;
		for (ip = identtab[i]; ip != 0; ip = ip->i_next)
			printf("%-14s %1d %3d\n", ip->i_name, ip->i_type, ip->i_i.i_regno);
	}

	for (i = 2; i < nmachregs; i++)
	{
		register reginfo rp;

		rp = machregs[i];
		printf("%s = (\"%s\", %d", rp->rname, rp->rrepr, rp->rsize);
		for (j = 0; j < MAXMEMBERS; j++)
			if (rp->rmembers[j] != 0)
				printf(", %s", machregs[rp->rmembers[j]]->rname);
		printf(")");
		for (j = 0; j < nprops; j++)
			if (rp->rprop[j >> 4] & (1 << (j & 017)))
				printf(", %s", machprops[j].propname->i_name);
		printf(".\n");
	}
}

void out(int n)
{

	assert(n >= 0);
	if (n < 128)
		outbyte(n);
	else
	{
		outbyte(n / 256 + 128);
		outbyte(n % 256);
	}
}

void outbyte(int n)
{

	fprintf(cfile, "%d, ", n & BMASK);
	codebytes++;
}

void pat(int n)
{

	assert(n >= 0);
	if (n < 128)
		patbyte(n);
	else
	{
		patbyte(n / 256 + 128);
		patbyte(n % 256);
	}
}

void patshort(int n)
{

	patbyte(n & BMASK);
	patbyte(n >> BSHIFT);
}

void patbyte(int n)
{
	chktabsiz(npatbytes, MAXPATTERN, "Pattern table");
	pattern[npatbytes++] = n;
}

int max(int a, int b)
{
	if (a > b)
		return (a);
	return (b);
}

int yywrap(void)
{
	return 1;
}

