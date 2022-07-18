#ifndef NORCSID
static char rcsidp5[] = "$Id$";
#endif

#include <string.h>
#include "alloc.h"
#include "parser.h"
#include "Lpars.h"

FILE *ofile;

PRIVATE void openofile(char *);
PRIVATE void installofile(void);
PRIVATE void UNLINK(char *);
PRIVATE void RENAME(char *, char *);
PRIVATE void increase_next(unsigned int);
PRIVATE void store_row(int, register int *);
PRIVATE void outdfa(void);
PRIVATE void outmnems(struct mnems);
PRIVATE int sametest(int, int, struct exp_node *, struct exp_node *);
PRIVATE int samerepl(int, int, struct mnems, struct mnems);
PRIVATE int samecode(int, int);
PRIVATE void outdotrans(void);
PRIVATE void outoneaction(int, struct action *);
PRIVATE void outrepl(int, struct mnems);
PRIVATE void outexp(struct exp_node *, int);
PRIVATE void outext(struct exp_node *);
PRIVATE void outop(int op);

void outputnopt(void)
{
	openofile("dfa.c");
	outdfa();
	installofile();
	openofile("trans.c");
	outdotrans();
	installofile();
	openofile("incalls.r");
	outputincalls();
	installofile();
}

static char ofilename[80];
static char ofiletemp[80];

PRIVATE void openofile(char *filename)
{
	strcpy(ofilename, filename);
	strcpy(ofiletemp, filename);
	strcat(ofiletemp, ".new");
	if ((ofile = fopen(ofiletemp, "wb")) == NULL)
	{
		fprintf(stderr, "Fatal Error: cannot open output file %s\n", ofiletemp);
		sys_stop(S_EXIT);
	}
}

PRIVATE void installofile(void)
{
	/*
	 * if contents of newly generated ofiletemp is different
	 * from that of ofilename then copy over old file else
	 * delete newly generated file
	 */
	register FILE *f1, *f2;
	register int c1, c2;
	fclose(ofile);
	if ((f1 = fopen(ofiletemp, "rb")) == NULL)
	{
		fprintf(stderr, "Fatal Error: cannont reopen file %s\n", ofiletemp);
		sys_stop(S_EXIT);
	}
	if ((f2 = fopen(ofilename, "rb")) == NULL)
	{
		fclose(f1);
		RENAME(ofiletemp, ofilename);
		return;
	}
	do
	{
		c1 = getc(f1);
		c2 = getc(f2);
	} while (c1 == c2 && c1 != EOF);
	fclose(f1);
	fclose(f2);
	if (c1 != c2)
	{
		RENAME(ofiletemp, ofilename);
	}
	else
		UNLINK(ofiletemp);
}

PRIVATE void UNLINK(char *x)
{
	/* Must remove the file "x" */
	remove(x); /* systemcall to remove file */
}

PRIVATE void RENAME(char *x, char*y)
{
	/* Must move the file "x" to the file "y" */
	if (rename(x, y) != 0)
	{
		fprintf(stderr, "Cannot rename to %s", y);
		sys_stop(S_EXIT);
	}
}

# define MAXOPCODE 255
# define EMPTY     -1

int *next, *check, *base;
unsigned currsize; /* current size of next and check arrays */
int maxpos = 0; /* highest used position in these arrayes */

PRIVATE void increase_next(unsigned int size)
{
	/* realloc arrays next and check so they are at least
	 * of size 'size'
	 */
	unsigned newsize = currsize;
	register unsigned int i;
	do
	{
		newsize *= 2;
	} while (newsize < size);
	printf("Note: Extending next/check arrays from %d to %d\n", currsize,
			newsize);
	next = (int *) Realloc(next, newsize);
	check = (int *) Realloc(check, newsize);
	/* clear ends of new arrays */
	for (i = currsize; i < newsize; i++)
		next[i] = check[i] = EMPTY;
	currsize = newsize;
}

PRIVATE void store_row(int state, register int *row)
{
	/* find a place to store row in arrays */
	register int b, i, o;
	register int *n = next;
	b = 0;
	for (;;)
	{
		/* look for first possible place to store it */
		for (i = 0; i < MAXOPCODE; i++)
		{
			if (row[i])
			{
				if ((o = b + i) > currsize)
					increase_next(o);
				if (n[o] != EMPTY)
					goto nextpos;
			}
		}
		/* found a place */
		base[state] = b;
		for (i = 0; i < MAXOPCODE; i++)
			if (row[i])
			{
				if ((o = b + i) > maxpos)
					maxpos = o;
				next[o] = row[i];
				check[o] = state;
			}
		return;
		nextpos: ++b;
	}
}

PRIVATE void outdfa(void)
{
	register int s, i;
	register struct state *p;
	int nout, ncpy, ngto;
	int row[MAXOPCODE];
	int numinrow;
	int numentries = 0;

	fprintf(ofile, "#include \"nopt.h\"\n");
	fprintf(ofile, "\n");
	fprintf(ofile, "int OO_maxreplacement = %d;\n", maxreplacement);
	fprintf(ofile, "\n");

	/* find how many entries in dfa */
	for (s = 0; s <= higheststate; s++)
		for (p = states[s]; p != (struct state *) NULL; p = p->next)
			numentries++;
	/* start with next and check arrays twice this size */
	currsize = 2 * numentries;
	next = (int *) Malloc(currsize * sizeof(int));
	check = (int *) Malloc(currsize * sizeof(int));
	base = (int *) Malloc(((unsigned) (higheststate + 1)) * sizeof(int));
	/* fill next array with EMPTY */
	for (i = 0; i < currsize; i++)
		check[i] = next[i] = EMPTY;
	for (s = 0; s <= higheststate; s++)
	{
		/* empty row */
		for (i = 0; i < MAXOPCODE; i++)
			row[i] = 0;
		numinrow = 0;
		/* fill in non zero entries */
		for (p = states[s]; p != (struct state *) NULL; p = p->next)
		{
			numinrow++;
			row[p->op->id_opcode] = p->goto_state;
		}
		/* look for a place to store row */
		if (numinrow)
			store_row(s, row);
		else
			base[s] = EMPTY;
	}
	/* give some statistics on size of dfa */
	printf("Number of patterns: %d\n", numpatterns);
	printf("Longest pattern: %d\n", maxpattern);
	printf("Longest replacement: %d\n", maxreplacement);
	printf("Dfa contains %d states and %d distinct state/opcode pairs\n",
			higheststate + 1, numentries);
	printf("Compacted using row displacement into %d entries\n", maxpos);
	/* output the arrays */
	fprintf(ofile, "struct dfa OO_checknext[] = {\n");
	for (i = 0; i <= maxpos; i++)
	{
		fprintf(ofile, "\t/* %4d */\t", i);
		fprintf(ofile, "{%4d,%4d},\n", check[i], next[i]);
	}
	fprintf(ofile, "};\n\n");
	fprintf(ofile, "struct dfa *OO_base[] = {\n");
	for (s = 0; s <= higheststate; s++)
	{
		fprintf(ofile, "\t/* %4d: ", s);
		outmnems(patterns[s]);
		fprintf(ofile, "*/\t");
		if (base[s] == EMPTY)
			fprintf(ofile, "0,\n");
		else
			fprintf(ofile, "&OO_checknext[%4d],\n", base[s]);
	}
	fprintf(ofile, "};\n\n");
	fprintf(ofile, "struct dodefault OO_default[] = {\n");
	for (s = 0; s <= higheststate; s++)
	{
		fprintf(ofile, "\t/* %4d: ", s);
		outmnems(patterns[s]);
		fprintf(ofile, "*/\t");
		findfail(s, &nout, &ncpy, &ngto);
		fprintf(ofile, "{%4d,%4d},\n", nout, ngto);
	}
	fprintf(ofile, "};\n\n");
}

PRIVATE void outmnems(struct mnems l)
{
	int i;
	for (i = 1; i <= l.m_len; i++)
		fprintf(ofile, "%s ", l.m_elems[i - 1]->op_code->id_text);
}

PRIVATE int sametest(int s1, int s2, struct exp_node *e1, struct exp_node *e2)
{
	/* return 1 if tests are identical */
	if (e1)
	{
		if (!e2)
			return 0;
		if (e1->node_type != e2->node_type)
			return 0;
		switch (e1->node_type)
		{
			case LOGAND:
			case LOGOR:
			case BITAND:
			case BITOR:
			case XOR:
			case MINUS:
			case PLUS:
			case TIMES:
			case DIV:
			case MOD:
			case EQ:
			case NE:
			case LT:
			case LE:
			case GT:
			case GE:
			case LSHIFT:
			case RSHIFT:
			case COMMA:
			case SAMESIGN:
			case SFIT:
			case UFIT:
			case ROTATE:
			case SAMEEXT:
			case SAMENAM:
				return (sametest(s1, s2, e1->exp_left, e2->exp_left)
						&& sametest(s1, s2, e1->exp_right, e2->exp_right));
			case NOT:
			case COMP:
			case UPLUS:
			case UMINUS:
				return sametest(s1, s2, e1->exp_left, e2->exp_left);
			case DEFINED:
			case UNDEFINED:
			case INT:
				return (e1->leaf_val == e2->leaf_val);
			case PATARG:
				/* depends on pattern !! */
				if (e1->leaf_val != e2->leaf_val)
					return 0;
				return (patterns[s1].m_elems[e1->leaf_val - 1]->op_code->id_argfmt
						== patterns[s2].m_elems[e2->leaf_val - 1]->op_code->id_argfmt);
			case PSIZE:
			case WSIZE:
			case DWSIZE:
				return 1;

		}
		/*NOTREACHED*/
	}
	else
		return (e2 == 0);
}

PRIVATE int samerepl(int s1, int s2, struct mnems r1, struct mnems r2)
{
	/* return 1 if replacements are identical */
	register int i;
	register struct mnem_elem *m1, *m2;
	if (r1.m_len != r2.m_len)
		return 0; /* different length */
	for (i = 0; i < r1.m_len; i++)
	{
		m1 = r1.m_elems[i];
		m2 = r2.m_elems[i];
		if (m1->op_code != m2->op_code)
			return 0;
		if (!sametest(s1, s2, m1->arg, m2->arg))
			return 0;
	}
	return 1;
}

PRIVATE int samecode(int s1, int s2)
{
	/* return 1 if replacement code of state s1 and s2 are identical */
	register struct action *a1, *a2;
	if (patterns[s1].m_len != patterns[s2].m_len)
		return 0;
	a1 = actions[s1];
	a2 = actions[s2];
	while (a1)
	{
		if (!a2)
			return 0; /* a2 is shorter */
		if (!samerepl(s1, s2, a1->replacement, a2->replacement))
			return 0;
		if (!sametest(s1, s2, a1->test, a2->test))
			return 0;
		a1 = a1->next;
		a2 = a2->next;
	}
	if (a2)
		return 0; /* a2 is longer */
	return 1;
}

PRIVATE void outdotrans(void)
{
	register int s, t;
	struct action *a;
	int seennontested;
	int *farray;
	fprintf(ofile, "#include \"nopt.h\"\n\n");
	/* keep track of which procedure used for each state */
	farray = (int *) Malloc((unsigned) (higheststate + 1) * sizeof(int));
	for (s = 0; s <= higheststate; s++)
		farray[s] = EMPTY;
	/* output the functions avoiding duplicates */
	for (s = 0; s <= higheststate; s++)
	{
		if (actions[s] != (struct action *) NULL)
		{
			/* first look for a previous identical function */
			for (t = 0; t < s; t++)
			{
				if (actions[t] != (struct action *) NULL && samecode(s, t))
				{
					/* use state 't' instead */
					farray[s] = t;
					goto next_func;
				}
			}
			/* no identical function so make new one */
			farray[s] = s;
			fprintf(ofile, "\nstatic void do%dtrans(void) {\n", s);
			fprintf(ofile, "\tregister p_instr patt = OO_patternqueue;\n");
			fprintf(ofile, "\t/* ");
			outmnems(patterns[s]);
			fprintf(ofile, " */\n");
			seennontested = 0;
			for (a = actions[s]; a != (struct action *) NULL; a = a->next)
			{
				if (a->test != (struct exp_node *) NULL)
				{
					fprintf(ofile, "\tif(");
					outexp(a->test, s);
					fprintf(ofile, ") {\n");
					outoneaction(s, a);
					fprintf(ofile, "\t\treturn;\n");
					fprintf(ofile, "\t}\n");
				}
				else
				{
					if (seennontested)
					{
						fprintf(stderr,
								"parser: more than one untested action on state %d\n",
								s);
						nerrors++;
					}
					seennontested++;
					outoneaction(s, a);
				}
			}
			fprintf(ofile, "}\n");
		}
		next_func: continue;
	}
	/* output the array itself */
	fprintf(ofile, "\n\nvoid (*OO_ftrans[])(void)=\n{\n");
	for (s = 0; s <= higheststate; s++)
	{
		if (farray[s] != EMPTY)
			fprintf(ofile, "\tdo%dtrans,\n", farray[s]);
		else
			fprintf(ofile, "\t0,\n");
	}
	fprintf(ofile, "};\n");
}

PRIVATE void outoneaction(int s, struct action *a)
{
	fprintf(ofile, "\t\t/* -> ");
	outmnems(a->replacement);
	fprintf(ofile, " */\n");
	fprintf(ofile, "#ifdef STATS\n");
	fprintf(ofile, "\t\tif(OO_wrstats) fprintf(stderr,\"%d\\n\");\n",
			a->linenum);
	fprintf(ofile, "#endif\n");
	outrepl(s, a->replacement);
	findworst(patterns[s], a->replacement);
}

PRIVATE void outrepl(int state, struct mnems repl)
{
	/*  Contruct <repl>=r1 r2 ... rn and put on output queue.
	 */
	int n = repl.m_len;
	int i;
	for (i = 1; i <= n; i++)
	{
		struct mnem_elem *ri = repl.m_elems[i - 1];
		char *mnem = ri->op_code->id_text;
		switch (ri->op_code->id_argfmt)
		{
			case NOARG:
				fprintf(ofile, "\t\tEM_Rop(op_%s);\n", mnem);
				break;
			case CST:
				fprintf(ofile, "\t\tEM_Rcst(op_%s,", mnem);
				fprintf(ofile, "(arith)");
				outexp(ri->arg, state);
				fprintf(ofile, ");\n");
				break;
			case CSTOPT:
				if (ri->arg)
				{
					fprintf(ofile, "\t\tEM_Rcst(op_%s,", mnem);
					fprintf(ofile, "(arith)");
					outexp(ri->arg, state);
				}
				else
				{
					fprintf(ofile, "\t\tEM_Rnarg(op_%s);\n", mnem);
				}
				fprintf(ofile, ");\n");
				break;
			case LAB:
				fprintf(ofile, "\t\tEM_Rilb(op_%s,", mnem);
				outexp(ri->arg, state);
				fprintf(ofile, ");\n");
				break;
			case DEFILB:
				fprintf(ofile, "\t\tEM_Rdefilb(op_%s,", mnem);
				outexp(ri->arg, state);
				fprintf(ofile, ");\n");
				break;
			case PNAM:
				fprintf(ofile, "\t\tEM_Rpro(op_%s,", mnem);
				outexp(ri->arg, state);
				fprintf(ofile, ");\n");
				break;
			case EXT:
				fprintf(ofile, "\t\tOO_mkext(GETNXTREPL(), op_%s,", mnem);
				outexp(ri->arg, state);
				fprintf(ofile, ");\n");
				break;
		}
	}
}

PRIVATE void outexp(struct exp_node *e, int state)
{
	switch (e->node_type)
	{
		case LOGAND:
		case LOGOR:
		case BITAND:
		case BITOR:
		case XOR:
		case MINUS:
		case PLUS:
		case TIMES:
		case DIV:
		case MOD:
		case EQ:
		case NE:
		case LT:
		case LE:
		case GT:
		case GE:
		case LSHIFT:
		case RSHIFT:
			fprintf(ofile, "(");
			outexp(e->exp_left, state);
			outop(e->node_type);
			outexp(e->exp_right, state);
			fprintf(ofile, ")");
			break;
		case NOT:
		case COMP:
		case UPLUS:
		case UMINUS:
			fprintf(ofile, "(");
			outop(e->node_type);
			outexp(e->exp_left, state);
			fprintf(ofile, ")");
			break;
		case DEFINED:
			fprintf(ofile, "DEFINED(patt[%d])", e->leaf_val - 1);
			break;
		case UNDEFINED:
			fprintf(ofile, "!DEFINED(patt[%d])", e->leaf_val - 1);
			break;
		case COMMA:
			outext(e->exp_left);
			fprintf(ofile, ",");
			fprintf(ofile, "(arith)");
			outexp(e->exp_right, state);
			break;
		case SAMESIGN:
		case SFIT:
		case UFIT:
		case ROTATE:
			outop(e->node_type);
			fprintf(ofile, "(arith)");
			outexp(e->exp_left, state);
			fprintf(ofile, ",");
			fprintf(ofile, "(arith)");
			outexp(e->exp_right, state);
			fprintf(ofile, ")");
			break;
		case SAMEEXT:
		case SAMENAM:
			outop(e->node_type);
			outext(e->exp_left);
			fprintf(ofile, ",");
			outext(e->exp_right);
			fprintf(ofile, ")");
			break;
		case PATARG:
			switch (patterns[state].m_elems[e->leaf_val - 1]->op_code->id_argfmt)
			{
				case NOARG:
					fprintf(stderr, "error: mnem %d has no argument\n",
							e->leaf_val);
					nerrors++;
					break;
				case CST:
				case CSTOPT:
					fprintf(ofile, "CST(patt[%d])", e->leaf_val - 1);
					break;
				case LAB:
					fprintf(ofile, "LAB(patt[%d])", e->leaf_val - 1);
					break;
				case DEFILB:
					fprintf(ofile, "DEFILB(patt[%d])", e->leaf_val - 1);
					break;
				case PNAM:
					fprintf(ofile, "PNAM(patt[%d])", e->leaf_val - 1);
					break;
				case EXT:
					fprintf(ofile, "OO_offset(patt+%d)", e->leaf_val - 1);
					break;
			}
			break;
		case PSIZE:
			fprintf(ofile, "OO_PSIZE");
			break;
		case WSIZE:
			fprintf(ofile, "OO_WSIZE");
			break;
		case DWSIZE:
			fprintf(ofile, "OO_DWSIZE");
			break;
		case INT:
			fprintf(ofile, "%d", e->leaf_val);
			break;
	}
}

PRIVATE void outext(struct exp_node *e)
{
	if (e->node_type != PATARG)
	{
		fprintf(stderr, "Internal error in outext of parser\n");
		nerrors++;
	}
	fprintf(ofile, "patt+%d", e->leaf_val - 1);
}

PRIVATE void outop(int op)
{
	switch (op)
	{
		case LOGAND:
			fprintf(ofile, "&&");
			break;
		case LOGOR:
			fprintf(ofile, "||");
			break;
		case BITAND:
			fprintf(ofile, "&");
			break;
		case BITOR:
			fprintf(ofile, "|");
			break;
		case XOR:
			fprintf(ofile, "^");
			break;
		case MINUS:
			fprintf(ofile, "-");
			break;
		case PLUS:
			fprintf(ofile, "+");
			break;
		case TIMES:
			fprintf(ofile, "*");
			break;
		case DIV:
			fprintf(ofile, "/");
			break;
		case MOD:
			fprintf(ofile, "%%");
			break;
		case EQ:
			fprintf(ofile, "==");
			break;
		case NE:
			fprintf(ofile, "!=");
			break;
		case LT:
			fprintf(ofile, "<");
			break;
		case LE:
			fprintf(ofile, "<=");
			break;
		case GT:
			fprintf(ofile, ">");
			break;
		case GE:
			fprintf(ofile, ">=");
			break;
		case LSHIFT:
			fprintf(ofile, "<<");
			break;
		case RSHIFT:
			fprintf(ofile, ">>");
			break;
		case NOT:
			fprintf(ofile, "!");
			break;
		case COMP:
			fprintf(ofile, "~");
			break;
		case UPLUS:
			fprintf(ofile, "+");
			break;
		case UMINUS:
			fprintf(ofile, "-");
			break;
		case SAMESIGN:
			fprintf(ofile, "OO_signsame(");
			break;
		case SFIT:
			fprintf(ofile, "OO_sfit(");
			break;
		case UFIT:
			fprintf(ofile, "OO_ufit(");
			break;
		case ROTATE:
			fprintf(ofile, "OO_rotate(");
			break;
		case SAMEEXT:
			fprintf(ofile, "OO_extsame(");
			break;
		case SAMENAM:
			fprintf(ofile, "OO_namsame(");
			break;
	}
}
