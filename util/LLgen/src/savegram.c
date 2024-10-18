/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * All rights reserved.
 */

#ifdef NON_CORRECTING

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * savegram.c
 * Save the input grammar for non-correcting error recovery
 *
 * Grammar rules are `flattened' by introducing anonymous nonterminals.
 * [B]? becomes X; X: B | {empty}
 * [B]+ becomes X: B Y; Y: X | {empty}
 * [B]* becomes X; X: B X | {empty}
 * [B | C] becomes X; X: B | C
 * [B | C]* becomes X; X: B X | C X | {empty}  etc.
 */

#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "extern.h"
#include "fileio.h"
#include "assert.h"
#include "sets.h"

#define LLALT 9999

static int nt_highest;
extern int nbytes;
extern p_mem alloc();
extern p_set start_firsts;
extern p_set setalloc();
extern p_gram search();

STATIC void save_rule();
STATIC void save_set();

/* t_list will contain terms to be `flattened' */
static struct t_list
{
	p_term term;
	int t_nt_num;
}* t_list;

/* Subparse list will contain symbols in %substart */
static struct subparse_list
{
	p_gram sub_action;
	int sub_nt_num;
}* sub_list;

/* Index in t_list */
static int t_list_index;

/* Index in subparse_list */
static int sub_list_index;

/* File to save grammar to */
static FILE* fgram;

/* Nonterminal number to simulate parsers that get called in actions
   used when LLgen called with -n -s options */
int act_nt;

void save_grammar(FILE* f)
{
	/*
	 * Save the grammar
	 */
	register p_nont p;
	register p_start st;
	register int nt_nr;

	fgram = f;

	/* Compute highest nonterminal nr. */
	nt_highest = nnonterms + assval - 1;

	/* Generate some constants in the grammar file */

	/* Allocate terms list */
	t_list = (struct t_list*)alloc(nterms * sizeof(struct t_list));
	t_list_index = 0;

	sub_list = (struct subparse_list*)alloc(nsubstarts * sizeof(struct subparse_list));

	fputs("static ", fgram);
	fputs((prefix ? prefix : "LL"), fgram);
	fputs("grammar[] = {\n", fgram);

	/* Check if -n -s option is on */
	if (subpars_sim)
	{

		/* Allocate action simulation nt */

		act_nt = ++nt_highest;

		/* write simualtion rule */
		fprintf(fgram, "/* Simulation rule */\n");
		fprintf(fgram, "%d,\n", act_nt);

		/* Put a firstset and a fake followset */
		/* Followset optimization is not implemented for
		   -s because it would be hard, and does not
		   bring enough improvement to jutify the effort
		 */
		save_set(start_firsts);
		save_set(start_firsts);
		/* Simulation rule procudes empty */
		fprintf(fgram, "%d,\n", 1);
		for (st = start; st; st = st->ff_next)
		{
			fprintf(fgram, "%d, %d, %d, \n", st->ff_nont + assval, act_nt, LLALT);
		}
		fprintf(fgram, "%d, \n", 0);
	}

	/* Now process all rules */
	for (p = nonterms, nt_nr = assval; p < maxnt; p++, nt_nr++)
	{
		fprintf(fgram, "/* nr. %d %s */\n", nt_nr, p->n_name);
		fprintf(fgram, "%d, ", nt_nr);
		if (!p->n_rule)
		{ /* undefined */
			f_input = p->n_string;
			error(p->n_lineno, "Nonterminal %s not defined", p->n_name);
		}

		/* Save the first_set and follow set */
		save_set(p->n_nc_first);
		save_set(p->n_nc_follow);

		if (p->n_flags & EMPTY)
			fprintf(fgram, "%d,\n", 1);
		else
			fprintf(fgram, "%d,\n", 0);

		save_rule(p->n_rule, 0);

		fprintf(fgram, "%d,\n", 0);
	}

	/* Resolve terms, they are on t_list */

	fprintf(fgram, "/* Fresh nonterminals */\n");

	{
		int i;
		for (i = 0; i < t_list_index; i++)
		{

			/* Terms of the form [] without + ? * or number produce
			 a NIL pointer in the term-list */
			if ((t_list + i)->term == (struct term*)0)
			{
				continue;
			}

			fprintf(fgram, "%d, ", (t_list + i)->t_nt_num);

			/* Save the first and follow sets */

			save_set((t_list + i)->term->t_nc_first);
			save_set((t_list + i)->term->t_nc_follow);

			/* NOTE: A VARIABLE REPETITION COUNT TERMS RULE IS NOT
			   ALLOWED TO PRODUCE EMPTY IN LLGEN
			 */

			switch (r_getkind((t_list + i)->term))
			{
				case FIXED:
					/* Already done by repeating new nonterminal */

					/* FIXED term-rule may produce empty */
					if (empty((t_list + i)->term->t_rule))
						fprintf(fgram, "%d,\n", 1);
					else
						fprintf(fgram, "%d,\n", 0);

					save_rule((t_list + i)->term->t_rule, 0);
					fprintf(fgram, "%d,\n", 0);
					break;
				case STAR:
					/* Save the rule, appending the new lhs for this rule */

					/* Star rules always produce empty */
					fprintf(fgram, "1,\n");

					save_rule((t_list + i)->term->t_rule, (t_list + i)->t_nt_num);
					fprintf(fgram, "%d,\n%d,\n", LLALT, 0);
					/* ALT EMPTY*/
					break;
				case PLUS:
					/* Save the rule appending a fresh nonterminal */

					fprintf(fgram, "%d,\n", 0);

					save_rule((t_list + i)->term->t_rule, ++nt_highest);
					fprintf(fgram, "%d,\n", 0); /* EOR */
					fprintf(fgram, "%d, ", nt_highest);
					/* First set of the extra nonterm is same as
					   for the term */
					/* Except that the new nonterm also produces empty ! */
					save_set((t_list + i)->term->t_nc_first);
					save_set((t_list + i)->term->t_nc_follow);
					fprintf(fgram, "1,\n");
					fprintf(fgram, "%d, ", (t_list + i)->t_nt_num);
					fprintf(fgram, "%d,\n%d,\n", LLALT, 0); /* ALT EMPTY */
					break;
				case OPT:
					fprintf(fgram, "1,\n");
					save_rule((t_list + i)->term->t_rule, 0);
					fprintf(fgram, "%d,\n%d,\n", LLALT, 0); /* ALT EMPTY */
					break;
			}
		}
	}

	/* Resolve %substarts */
	if (!subpars_sim)
	{
		int i, s, check;
		p_start ff, gg;
		p_set temp_set;

		for (i = 0; i < sub_list_index; i++)
		{
			fprintf(fgram, "%d, ", (sub_list + i)->sub_nt_num);
			/* Compute the first set */
			temp_set = setalloc();
			for (ff = g_getsubparse((sub_list + i)->sub_action); ff; ff = ff->ff_next)
			{
				s = setunion(temp_set, (&nonterms[ff->ff_nont])->n_first);
				check = 0;
				for (gg = start; gg; gg = gg->ff_next)
					if (ff->ff_nont == gg->ff_nont)
						check = 1;
				if (check == 0)
					warning(
					    (sub_list + i)->sub_action->g_lineno, "\"%s\" is not a startsymbol",
					    (&nonterms[ff->ff_nont])->n_name);
			}
			save_set(temp_set);
			save_set(temp_set);
			free(temp_set);

			/* Produces empty */
			fprintf(fgram, "1,\n");

			ff = g_getsubparse((sub_list + i)->sub_action);

			for (; ff; ff = ff->ff_next)
				fprintf(
				    fgram, "%d, %d, %d, \n", ff->ff_nont + assval, (sub_list + i)->sub_nt_num,
				    LLALT);
			fprintf(fgram, "%d, \n", 0);
		}
	}

	fprintf(fgram, "%d\n};\n", 0);
	fprintf(fgram, "#define LLNNONTERMINALS %d\n", nt_highest - assval + 1);
}

STATIC void save_rule(register p_gram p, int tail)
{
	/*
	 Walk through rule p, saving it. The non-terminal tail is
	 appended to the rule. It needs to be appended in this function
	 to process alt-rules correctly. Tail == 0 means don't append.
	 */

	int in_alt;
	int illegal_num;
	/* Processing an alt needs some special care. When processing the
	   first alternative, we don't want to write the alt-code;
	   When appending something to the alt, it needs to be appended to
	   every alternative and not at the end of the rule.
	*/

	/* Look up the ILLEGAL token number */
	illegal_num = tokens[g_getcont(illegal_gram)].t_tokno;

	in_alt = 0;
	for (;;)
	{
		switch (g_gettype(p))
		{
			case ALTERNATION:
				if (in_alt)
					fprintf(fgram, "%d,\n", LLALT);
				else
					in_alt = 1;
				save_rule(g_getlink(p)->l_rule, tail);
				break;
			case TERM:
				/* Make entry in term list */
				(t_list + t_list_index)->term = g_getterm(p);
				/* Test for [] without specifier */
				if (g_getterm(p) == (struct term*)0)
				{
					t_list_index++;
					break;
				}
				(t_list + t_list_index++)->t_nt_num = ++nt_highest;
				fprintf(fgram, "%d, ", nt_highest);
				/* Check if repetition,	 if so handle here */
				if (r_getkind(g_getterm(p)) == FIXED)
				{
					int k;
					for (k = 1; k < r_getnum(g_getterm(p)); k++)
						fprintf(fgram, "%d, ", nt_highest);
				}
				break;
			case NONTERM:
				fprintf(fgram, "%d, ", g_getcont(p) + assval);
				break;
			case TERMINAL:
				if (g_getcont(p) == g_getcont(illegal_gram))
				{
					/* %illegal. Ignore. */
					break;
				}
				if (p->g_erroneous)
					fprintf(fgram, "%d, ", illegal_num);
				else
					fprintf(fgram, "%d, ", tokens[g_getcont(p)].t_tokno);
				break;
			case LITERAL:
				if (p->g_erroneous)
					fprintf(fgram, "%d, ", illegal_num);
				else
					fprintf(fgram, "%d, ", tokens[g_getcont(p)].t_tokno);
				break;
			case ACTION:
				if (subpars_sim)
				{
					fprintf(fgram, "%d, ", act_nt);
				}
				else if (g_getsubparse(p))
				{
					/* Allocate nonterminal that will simulate
					   subparser
					 */
					(sub_list + sub_list_index)->sub_nt_num = ++nt_highest;
					(sub_list + sub_list_index++)->sub_action = p;

					fprintf(fgram, "%d, ", nt_highest);
				}
				break;
			case EORULE:
				if ((!in_alt) && tail)
					/* If this rule is not an alt, append tail now.
					   If it is an alt, the recursive call of this function
					   has appended tail to each alternative
					 */
					fprintf(fgram, "%d, ", tail);
				return;
		}
		p++;
	}
}

STATIC void save_set(p_set p)
{
	register int k;
	register unsigned i;
	int j;

	j = nbytes;
	for (;;)
	{
		i = (unsigned)*p++;
		for (k = 0; k < sizeof(int); k++)
		{
			fprintf(fgram, "0%o,", (int)(i & 0377));
			i >>= 8;
			if (--j == 0)
			{
				fputs("\n", fgram);
				return;
			}
		}
	}
	/* NOTREACHED */
}
#endif
