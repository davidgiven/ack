/* LLgen generated code from source LLgen.g */
#include "Lpars.h"
#define LL_LEXI scanner
#define LLNOFIRSTS
#define LL_LEXI scanner
/* $Id$ */
#ifdef LL_DEBUG
#include <assert.h>
#include <stdio.h>
#define LL_assert(x)	assert(x)
#else
#define LL_assert(x)	/* nothing */
#endif

extern int LLsymb;

#define LL_SAFE(x)	/* Nothing */
#define LL_SSCANDONE(x)	if (LLsymb != x) LLsafeerror(x)
#define LL_SCANDONE(x)	if (LLsymb != x) LLerror(x)
#define LL_NOSCANDONE(x) LLscan(x)
#ifdef LL_FASTER
#define LLscan(x)	if ((LLsymb = LL_LEXI()) != x) LLerror(x)
#endif

extern unsigned int LLscnt[];
extern unsigned int LLtcnt[];
extern int LLcsymb;

#if LL_NON_CORR
extern int LLstartsymb;
#endif

#define LLsdecr(d)	{LL_assert(LLscnt[d] > 0); LLscnt[d]--;}
#define LLtdecr(d)	{LL_assert(LLtcnt[d] > 0); LLtcnt[d]--;}
#define LLsincr(d)	LLscnt[d]++
#define LLtincr(d)	LLtcnt[d]++

extern int LL_LEXI(void);
extern void LLread(void);
extern int LLskip(void);
extern int LLnext(int);
extern void LLerror(int);
extern void LLsafeerror(int);
extern void LLnewlevel(unsigned int *);
extern void LLoldlevel(unsigned int *);
#ifndef LL_FASTER
extern void LLscan(int);
#endif
#ifndef LLNOFIRSTS
extern int LLfirst(int, int);
#endif
#if LL_NON_CORR
extern void LLnc_recover(void);
#endif
# line 20 "LLgen.g"

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "io.h"
#include "extern.h"
#include "assert.h"
#include "cclass.h"

# ifndef NORCSID
static string	rcsid = "$Id$";
# endif
p_mem		alloc(), ralloc();
string		store();
p_gram		search();
long		ftell();

static int	acount;			/* count #of global actions */
static p_term t_list;
static int t_cnt;
static p_gram	alt_table;
static int	n_alts;
static int	max_alts;
#define ALTINCR 32

static p_gram	rule_table;
static int	n_rules;
static int	max_rules;
#define RULEINCR	32

/* Here are defined : */
STATIC void newnorder(int index);
STATIC void newtorder(int index);
STATIC void mkalt(p_gram prod, int condition,int lc,register p_gram res); 
STATIC void mkterm(p_gram prod, int flags,int lc,register p_gram result); 
STATIC p_gram copyrule(register p_gram p,int length);
/* and of course LLparse() */

STATIC void newnorder(int index) {
	static int porder;

	if (norder != -1) {
		nonterms[porder].n_next = index;
	}
	else	norder = index;
	porder = index;
	nonterms[porder].n_next = -1;
}

STATIC void newtorder(int index) {
	static int porder;

	if (torder != -1) {
		tokens[porder].t_next = index;
	}
	else	torder = index;
	porder = index;
	tokens[porder].t_next = -1;
}

void p_init(void)
{
	alt_table = (p_gram )alloc(ALTINCR*sizeof(t_gram));
	n_alts = 0;
	max_alts = ALTINCR;
	rule_table = (p_gram )alloc(RULEINCR*sizeof(t_gram));
	n_rules = 0;
	max_rules = RULEINCR;
}

static void LL1_def(void);
static void LL2_rule(void);
static void LL3_listel(void);
static void LL4_firsts(void);
static void LL5_productions(
# line 244 "LLgen.g"
p_gram *p) ;
static void LL6_simpleproduction(
# line 330 "LLgen.g"
p_gram *p ,register int *conflres) ;
static void LL7_elem(
# line 478 "LLgen.g"
register p_gram pres) ;
static void LL8_repeats(
# line 600 "LLgen.g"
int *kind ,int *cnt) ;
static void LL9_number(
# line 617 "LLgen.g"
int *t) ;
void
LL0_spec(
void
) {
LLsincr(0);
# line 94 "LLgen.g"
{	acount = 0; p_init(); }
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  EOFILE  */ 0 : ;
break;
default:{int LL_1=LLnext(0);
;if (!LL_1) {
break;
}
else if (LL_1 & 1) goto L_1;}
case /*  C_IDENT  */ 2 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_TOKEN  */ 8 : ;
case /*  C_START  */ 9 : ;
case /*  C_FIRST  */ 13 : ;
case /*  C_LEXICAL  */ 14 : ;
case /*  C_PREFIX  */ 15 : ;
case /*  C_ONERROR  */ 16 : ;
LL1_def();
LLread();
continue;
}
}
LLsdecr(0);
break;
}
# line 96 "LLgen.g"
{	/*
				 * Put an endmarker in temporary file
				 */
				putc('\0', fact);
				putc('\0', fact);
				free((p_mem) rule_table);
				free((p_mem) alt_table);
			}
}
static
void
LL1_def(
void
) {
# line 106 "LLgen.g"
	register string p; 
switch(LLcsymb) {
case /*  C_IDENT  */ 2 : ;
LL2_rule();
break;
case /*  C_TOKEN  */ 8 : ;
LLtincr(23);
LLtincr(24);
LL_SAFE(C_TOKEN);
LL3_listel();
LLread();
for (;;) {
goto L_4;
L_4 : {switch(LLcsymb) {
case /* ';' */ 24 : ;
break;
default:{int LL_2=LLnext(44);
;if (!LL_2) {
break;
}
else if (LL_2 & 1) goto L_4;}
case /* ',' */ 23 : ;
LL_SAFE(',');
LL3_listel();
LLread();
continue;
}
}
LLtdecr(23);
break;
}
LLtdecr(24);
LL_SSCANDONE(';');
break;
case /*  C_START  */ 9 : ;
LLtincr(23);
LLtincr(2);
LLtincr(24);
LL_SAFE(C_START);
LL_NOSCANDONE(C_IDENT);
# line 116 "LLgen.g"
{	p = store(lextoken.t_string); }
LLtdecr(23);
LL_NOSCANDONE(',');
LLtdecr(2);
LL_NOSCANDONE(C_IDENT);
# line 121 "LLgen.g"
{	/*
				 * Put the declaration in the list
				 * of start symbols
				 */
				register p_gram temp;
				register p_start ff;

				temp = search(NONTERM,lextoken.t_string,BOTH);
				ff = (p_start) alloc(sizeof(t_start));
				ff->ff_nont = g_getcont(temp);
				ff->ff_name = p;
				ff->ff_next = start;
				start = ff;
				while (ff = ff->ff_next) {
					if (! strcmp(p, ff->ff_name)) {
						error(linecount, "\"%s\" already used in a %%start", p);
						break;
					}
				}
			}
LLtdecr(24);
LL_NOSCANDONE(';');
break;
case /*  C_LEXICAL  */ 14 : ;
LLtincr(24);
LL_SAFE(C_LEXICAL);
LL_NOSCANDONE(C_IDENT);
# line 147 "LLgen.g"
{	if (!lexical) {
					lexical = store(lextoken.t_string);
				}
				else	error(linecount,"Duplicate %%lexical", NULL);
			}
LLtdecr(24);
LL_NOSCANDONE(';');
break;
case /*  C_PREFIX  */ 15 : ;
LLtincr(24);
LL_SAFE(C_PREFIX);
LL_NOSCANDONE(C_IDENT);
# line 157 "LLgen.g"
{	if (!prefix) {
					prefix = store(lextoken.t_string);
					if (strlen(prefix) > 6) {
						error(linecount,
							"%%prefix too long",NULL);
						prefix[6] = 0;
					}
				}
				else	error(linecount,"Duplicate %%prefix", NULL);
			}
LLtdecr(24);
LL_NOSCANDONE(';');
break;
case /*  C_ONERROR  */ 16 : ;
LLtincr(24);
LL_SAFE(C_ONERROR);
LL_NOSCANDONE(C_IDENT);
# line 169 "LLgen.g"
{
#ifdef NON_CORRECTING
				if (non_corr) {
					warning(linecount, "%%onerror conflicts with -n option", NULL);
				}
				else
#endif
				  if (! onerror) {
					onerror = store(lextoken.t_string);
				}
				else	error(linecount,"Duplicate %%onerror", NULL);
			}
LLtdecr(24);
LL_NOSCANDONE(';');
break;
default:
LL_SSCANDONE(C_ACTION);
# line 182 "LLgen.g"
{	acount++; }
break;
case /*  C_FIRST  */ 13 : ;
LL4_firsts();
break;
}
}
static
void
LL3_listel(
void
) {
LL_NOSCANDONE(C_IDENT);
# line 192 "LLgen.g"
{	p_gram temp = search(TERMINAL,lextoken.t_string,ENTERING);
				newtorder(g_getcont(temp));
				tokens[g_getcont(temp)].t_lineno = linecount;
			}
}
static
void
LL2_rule(
void
) {
# line 198 "LLgen.g"
	register p_nont p;
				p_gram rr;
				register p_gram temp;
			
LLtincr(6);
LLtincr(7);
LLtincr(25);
LLsincr(1);
LLtincr(24);
LL_SAFE(C_IDENT);
# line 205 "LLgen.g"
{	temp = search(NONTERM,lextoken.t_string,BOTH);
				p = &nonterms[g_getcont(temp)];
				if (p->n_rule) {
					error(linecount,
"Nonterminal %s already defined", lextoken.t_string);
				}
				/*
				 * Remember the order in which the nonterminals
				 * were defined. Code must be generated in that
				 * order to keep track with the actions on the
				 * temporary file
				 */
				newnorder(p - nonterms);
				p->n_count = acount;
				acount = 0;
				p->n_lineno = linecount;
				p->n_off = ftell(fact);
			}
LLread();
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  C_ACTION  */ 7 : ;
case /* ':' */ 25 : ;
LLtdecr(6);
break;
default:{int LL_3=LLnext(262);
;if (!LL_3) {
LLtdecr(6);
break;
}
else if (LL_3 & 1) goto L_1;}
case /*  C_PARAMS  */ 6 : ;
LLtdecr(6);
LL_SAFE(C_PARAMS);
# line 223 "LLgen.g"
{	if (lextoken.t_num > 0) {
					p->n_flags |= PARAMS;
					if (lextoken.t_num > 15) {
						error(linecount,"Too many parameters", NULL);
					}
					else	setntparams(p,lextoken.t_num);
				}
			}
LLread();
}
}
goto L_2;
L_2 : {switch(LLcsymb) {
case /* ':' */ 25 : ;
LLtdecr(7);
break;
default:{int LL_4=LLnext(263);
;if (!LL_4) {
LLtdecr(7);
break;
}
else if (LL_4 & 1) goto L_2;}
case /*  C_ACTION  */ 7 : ;
LLtdecr(7);
LL_SAFE(C_ACTION);
# line 232 "LLgen.g"
{	p->n_flags |= LOCALS; }
LLread();
}
}
LLtdecr(25);
LL_SCANDONE(':');
# line 234 "LLgen.g"
{ 	in_production = 1; }
LLread();
LLsdecr(1);
LL5_productions(
# line 235 "LLgen.g"
&rr);
LLtdecr(24);
LL_SCANDONE(';');
# line 236 "LLgen.g"
{	in_production = 0; }
# line 241 "LLgen.g"
{	nonterms[g_getcont(temp)].n_rule = rr;}
}
static
void
LL5_productions(
# line 244 "LLgen.g"
p_gram *p)  
{
# line 248 "LLgen.g"
	p_gram		prod;
		int		conflres = 0;
		int		t = 0;
		int		haddefault = 0;
		int		altcnt = 0;
		int		o_lc, n_lc;
	
LLtincr(26);
# line 255 "LLgen.g"
{	o_lc = linecount; }
LL6_simpleproduction(
# line 256 "LLgen.g"
p,&conflres);
# line 257 "LLgen.g"
{	if (conflres & DEF) haddefault = 1; }
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /* '|' */ 26 : ;
LLtdecr(26);
LLsincr(1);
LLsdecr(1);
LLtincr(26);
for (;;) {
LL_SAFE('|');
# line 259 "LLgen.g"
{	n_lc = linecount; }
LLread();
LL6_simpleproduction(
# line 260 "LLgen.g"
&prod,&t);
# line 261 "LLgen.g"
{	if (n_alts >= max_alts-2) {
					alt_table = (p_gram ) ralloc(
						(p_mem) alt_table,
						(unsigned)(max_alts+=ALTINCR)*sizeof(t_gram));
				}
				if (t & DEF) {
					if (haddefault) {
						error(n_lc,
		"More than one %%default in alternation", NULL);
					}
					haddefault = 1;
				}
				mkalt(*p,conflres,o_lc,&alt_table[n_alts++]);
				altcnt++;
				o_lc = n_lc;
				conflres = t;
				t = 0;
				*p = prod;
			}
goto L_4;
L_4 : {switch(LLcsymb) {
case /* ';' */ 24 : ;
case /* ']' */ 28 : ;
break;
default:{int LL_5=LLnext(124);
;if (!LL_5) {
break;
}
else if (LL_5 & 1) goto L_4;}
case /* '|' */ 26 : ;
continue;
}
}
LLtdecr(26);
break;
}
# line 280 "LLgen.g"
{	if (conflres & (COND|PREFERING|AVOIDING)) {
					error(n_lc,
		"Resolver on last alternative not allowed", NULL);
				}
				mkalt(*p,conflres,n_lc,&alt_table[n_alts++]);
				altcnt++;
				g_settype((&alt_table[n_alts]),EORULE);
				*p = copyrule(&alt_table[n_alts-altcnt],altcnt+1);
			}
break;
case /* ';' */ 24 : ;
case /* ']' */ 28 : ;
goto L_3;
L_3: ;
LLtdecr(26);
# line 290 "LLgen.g"
{	if (conflres & (COND|PREFERING|AVOIDING)) {
					error(o_lc,
		"No alternation conflict resolver allowed here", NULL);
				}
				/*
				if (conflres & DEF) {
					error(o_lc,
		"No %%default allowed here", NULL);
				}
				*/
			}
break;
default: if (LLskip()) goto L_2;
goto L_3;
}
# line 302 "LLgen.g"
{	n_alts -= altcnt; }
}
# line 304 "LLgen.g"


STATIC void mkalt(p_gram prod, int condition,int lc,register p_gram res) 
{
	/*
	 * Create an alternation and initialise it.
	 */
	register p_link		l;
	static p_link list;
	static int cnt;

	if (! cnt) {
		cnt = 50;
		list = (p_link) alloc(50 * sizeof(t_link));
	}
	cnt--;
	l = list++;
	l->l_rule = prod;
	l->l_flag = condition;
	g_setlink(res,l);
	g_settype(res,ALTERNATION);
	res->g_lineno = lc;
	nalts++;
}
static
void
LL6_simpleproduction(
# line 330 "LLgen.g"
p_gram *p ,register int *conflres)  
{
# line 331 "LLgen.g"
	t_gram		elem;
		int		elmcnt = 0;
		int		cnt, kind;
		int		termdeleted = 0;
	
LLtincr(19);
LLsincr(2);
LLtincr(22);
LLsincr(3);
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  C_IDENT  */ 2 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_IF  */ 10 : ;
case /*  C_AVOID  */ 17 : ;
case /*  C_PREFER  */ 18 : ;
case /*  C_SUBSTART  */ 20 : ;
case /*  C_ERRONEOUS  */ 21 : ;
case /*  C_ILLEGAL  */ 22 : ;
case /* ';' */ 24 : ;
case /* '|' */ 26 : ;
case /* '[' */ 27 : ;
case /* ']' */ 28 : ;
LLtdecr(19);
break;
default:{int LL_6=LLnext(275);
;if (!LL_6) {
LLtdecr(19);
break;
}
else if (LL_6 & 1) goto L_1;}
case /*  C_DEFAULT  */ 19 : ;
LLtdecr(19);
LL_SAFE(C_DEFAULT);
# line 336 "LLgen.g"
{	*conflres |= DEF; }
LLread();
}
}
goto L_2;
L_2 : {switch(LLcsymb) {
case /*  C_IDENT  */ 2 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_SUBSTART  */ 20 : ;
case /*  C_ERRONEOUS  */ 21 : ;
case /*  C_ILLEGAL  */ 22 : ;
case /* ';' */ 24 : ;
case /* '|' */ 26 : ;
case /* '[' */ 27 : ;
case /* ']' */ 28 : ;
LLsdecr(2);
break;
default:{int LL_7=LLnext(-8);
;if (!LL_7) {
LLsdecr(2);
break;
}
else if (LL_7 & 1) goto L_2;}
case /*  C_IF  */ 10 : ;
case /*  C_AVOID  */ 17 : ;
case /*  C_PREFER  */ 18 : ;
LLsdecr(2);
switch(LLcsymb) {
case /*  C_IF  */ 10 : ;
LL_SAFE(C_IF);
LL_NOSCANDONE(C_EXPR);
# line 342 "LLgen.g"
{	*conflres |= COND; }
break;
default:
LL_SAFE(C_PREFER);
# line 343 "LLgen.g"
{	*conflres |= PREFERING; }
break;
case /*  C_AVOID  */ 17 : ;
LL_SAFE(C_AVOID);
# line 344 "LLgen.g"
{	*conflres |= AVOIDING; }
break;
}
LLread();
}
}
goto L_6;
L_6 : {switch(LLcsymb) {
case /*  C_IDENT  */ 2 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_SUBSTART  */ 20 : ;
case /*  C_ERRONEOUS  */ 21 : ;
case /* ';' */ 24 : ;
case /* '|' */ 26 : ;
case /* '[' */ 27 : ;
case /* ']' */ 28 : ;
LLtdecr(22);
break;
default:{int LL_8=LLnext(278);
;if (!LL_8) {
LLtdecr(22);
break;
}
else if (LL_8 & 1) goto L_6;}
case /*  C_ILLEGAL  */ 22 : ;
LLtdecr(22);
LL_SAFE(C_ILLEGAL);
# line 346 "LLgen.g"
{
#ifdef NON_CORRECTING
				if (n_rules >= max_rules-2) {
					rule_table = (p_gram) ralloc(
						  (p_mem) rule_table,
						  (unsigned)(max_rules+=RULEINCR)*sizeof(t_gram));
				}
				elmcnt++;
				rule_table[n_rules++] =
				    *search(TERMINAL, "LLILLEGAL", BOTH);
				if (*conflres & DEF) {
					error(linecount, "%%illegal not allowed in %%default rule", NULL);
				}
#endif
			}
LLread();
}
}
for (;;) {
goto L_7;
L_7 : {switch(LLcsymb) {
case /* ';' */ 24 : ;
case /* '|' */ 26 : ;
case /* ']' */ 28 : ;
break;
default:{int LL_9=LLnext(-12);
;if (!LL_9) {
break;
}
else if (LL_9 & 1) goto L_7;}
case /*  C_IDENT  */ 2 : ;
case /*  C_NUMBER  */ 3 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_SUBSTART  */ 20 : ;
case /*  C_ERRONEOUS  */ 21 : ;
case /* '[' */ 27 : ;
case /* '?' */ 29 : ;
case /* '*' */ 30 : ;
case /* '+' */ 31 : ;
LLsincr(4);
LL7_elem(
# line 362 "LLgen.g"
&elem);
# line 363 "LLgen.g"
{	if (n_rules >= max_rules-2) {
					rule_table = (p_gram) ralloc(
						  (p_mem) rule_table,
						  (unsigned)(max_rules+=RULEINCR)*sizeof(t_gram));
				}
				kind = FIXED;
				cnt = 0;
			}
goto L_9; /* so that the label is used for certain */
L_9: ;
switch(LLcsymb) {
case /*  C_NUMBER  */ 3 : ;
case /* '?' */ 29 : ;
case /* '*' */ 30 : ;
case /* '+' */ 31 : ;
LLsdecr(4);
LL8_repeats(
# line 371 "LLgen.g"
&kind, &cnt);
# line 372 "LLgen.g"
{	if (g_gettype(&elem) != TERM) {
					rule_table[n_rules] = elem;
					g_settype((&rule_table[n_rules+1]),EORULE);
					mkterm(copyrule(&rule_table[n_rules],2),
					       0,
					       elem.g_lineno,
					       &elem);
				}
			}
break;
case /*  C_IDENT  */ 2 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_SUBSTART  */ 20 : ;
case /*  C_ERRONEOUS  */ 21 : ;
case /* ';' */ 24 : ;
case /* '|' */ 26 : ;
case /* '[' */ 27 : ;
case /* ']' */ 28 : ;
goto L_10;
L_10: ;
LLsdecr(4);
# line 382 "LLgen.g"
{ if (g_gettype(&elem) == TERM) {
				register p_term q = g_getterm(&elem);

				if (! (q->t_flags & RESOLVER) &&
				    g_gettype(q->t_rule) != ALTERNATION &&
				    g_gettype(q->t_rule) != EORULE) {
				    while (g_gettype(q->t_rule) != EORULE) {
					rule_table[n_rules++] = *q->t_rule++;
					elmcnt++;
					if (n_rules >= max_rules-2) {
					    rule_table = (p_gram) ralloc(
						  (p_mem) rule_table,
						  (unsigned)(max_rules+=RULEINCR)*sizeof(t_gram));
					}
				    }
				    elem = *--(q->t_rule);
				    n_rules--;
				    elmcnt--;
				    if (q == t_list - 1) {
					t_list--;
					nterms--;
					t_cnt++;
				    }
				    termdeleted = 1;
				}
			  }
			}
break;
default: if (LLskip()) goto L_9;
goto L_10;
}
# line 409 "LLgen.g"
{	if (!termdeleted && g_gettype(&elem) == TERM) {
					register p_term q;

					q = g_getterm(&elem);
					r_setkind(q,kind);
					r_setnum(q,cnt);
					if ((q->t_flags & RESOLVER) &&
					    (kind == PLUS || kind == FIXED)) {
						error(linecount,
		"%%while not allowed in this term", NULL);
					}
					/*
					 * A persistent fixed term is the same
					 * as a non-persistent fixed term.
					 * Should we complain?
					if ((q->t_flags & PERSISTENT) &&
					    kind == FIXED) {
						error(linecount,
							"Illegal %%persistent", NULL);
					}
					*/
				}
				termdeleted = 0;
				elmcnt++;
				rule_table[n_rules++] = elem;
			}
continue;
}
}
LLsdecr(3);
break;
}
# line 435 "LLgen.g"
{	register p_term q;

				g_settype((&rule_table[n_rules]),EORULE);
				*p = 0;
				n_rules -= elmcnt;
				if (g_gettype(&rule_table[n_rules]) == TERM &&
				    elmcnt == 1) {
					q = g_getterm(&rule_table[n_rules]);
					if (r_getkind(q) == FIXED &&
					    r_getnum(q) == 0) {
						*p = q->t_rule;
					}
				}
				if (!*p) *p = copyrule(&rule_table[n_rules],
						elmcnt+1);
			}
}
# line 452 "LLgen.g"


STATIC void mkterm(p_gram prod, int flags,int lc,register p_gram result) 
{
	/*
	 * Create a term, initialise it and return
	 * a grammar element containing it
	 */
	register p_term		q;

	if (! t_cnt) {
		t_cnt = 50;
		t_list = (p_term) alloc(50 * sizeof(t_term));
	}
	t_cnt--;
	q = t_list++;
	q->t_rule = prod;
	q->t_contains = 0;
	q->t_flags = flags;
	g_settype(result,TERM);
	g_setterm(result,q);
	result->g_lineno = lc;
	nterms++;
}
static
void
LL7_elem(
# line 478 "LLgen.g"
register p_gram pres)  
{
# line 479 "LLgen.g"
	register int	t = 0;
		p_gram		p1;
		int		ln;
		p_gram		pe;
#ifdef NON_CORRECTING
		int		erroneous = 0;
#endif
	
switch(LLcsymb) {
case /* '[' */ 27 : ;
LLtincr(11);
LLtincr(12);
LLsincr(1);
LLtincr(28);
LL_SAFE('[');
# line 487 "LLgen.g"
{	ln = linecount; }
LLread();
goto L_4;
L_4 : {switch(LLcsymb) {
case /*  C_IDENT  */ 2 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_IF  */ 10 : ;
case /*  C_PERSISTENT  */ 12 : ;
case /*  C_AVOID  */ 17 : ;
case /*  C_PREFER  */ 18 : ;
case /*  C_DEFAULT  */ 19 : ;
case /*  C_SUBSTART  */ 20 : ;
case /*  C_ERRONEOUS  */ 21 : ;
case /*  C_ILLEGAL  */ 22 : ;
case /* '|' */ 26 : ;
case /* '[' */ 27 : ;
case /* ']' */ 28 : ;
LLtdecr(11);
break;
default:{int LL_10=LLnext(267);
;if (!LL_10) {
LLtdecr(11);
break;
}
else if (LL_10 & 1) goto L_4;}
case /*  C_WHILE  */ 11 : ;
LLtdecr(11);
LL_SAFE(C_WHILE);
LL_NOSCANDONE(C_EXPR);
# line 488 "LLgen.g"
{	t |= RESOLVER; }
LLread();
}
}
goto L_5;
L_5 : {switch(LLcsymb) {
case /*  C_IDENT  */ 2 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_IF  */ 10 : ;
case /*  C_AVOID  */ 17 : ;
case /*  C_PREFER  */ 18 : ;
case /*  C_DEFAULT  */ 19 : ;
case /*  C_SUBSTART  */ 20 : ;
case /*  C_ERRONEOUS  */ 21 : ;
case /*  C_ILLEGAL  */ 22 : ;
case /* '|' */ 26 : ;
case /* '[' */ 27 : ;
case /* ']' */ 28 : ;
LLtdecr(12);
break;
default:{int LL_11=LLnext(268);
;if (!LL_11) {
LLtdecr(12);
break;
}
else if (LL_11 & 1) goto L_5;}
case /*  C_PERSISTENT  */ 12 : ;
LLtdecr(12);
LL_SAFE(C_PERSISTENT);
# line 490 "LLgen.g"
{	t |= PERSISTENT; }
LLread();
}
}
LLsdecr(1);
LL5_productions(
# line 492 "LLgen.g"
&p1);
LLtdecr(28);
LL_SCANDONE(']');
# line 493 "LLgen.g"
{
				mkterm(p1,t,ln,pres);
			}
LLread();
break;
case /*  C_IDENT  */ 2 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ERRONEOUS  */ 21 : ;
LLsincr(5);
goto L_6;
L_6 : {switch(LLcsymb) {
default:
break;
case /*  C_ERRONEOUS  */ 21 : ;
LL_SAFE(C_ERRONEOUS);
# line 497 "LLgen.g"
{
#ifdef NON_CORRECTING
					erroneous = 1;
#endif
				}
LLread();
}
}
goto L_8; /* so that the label is used for certain */
L_8: ;
switch(LLcsymb) {
case /*  C_IDENT  */ 2 : ;
goto L_9;
L_9: ;
LLsdecr(5);
LLtincr(6);
LL_SSCANDONE(C_IDENT);
# line 505 "LLgen.g"
{	pe = search(UNKNOWN,lextoken.t_string,BOTH);
				*pres = *pe;
#ifdef NON_CORRECTING
				if (erroneous) {
					if (g_gettype(pres) != TERMINAL){
						warning(linecount,
							"Erroneous only allowed on terminal", NULL);
						erroneous = 0;
					}
					else
						pres->g_erroneous = 1;
				}
#endif

			}
LLread();
goto L_10;
L_10 : {switch(LLcsymb) {
case /*  C_IDENT  */ 2 : ;
case /*  C_NUMBER  */ 3 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_SUBSTART  */ 20 : ;
case /*  C_ERRONEOUS  */ 21 : ;
case /* ';' */ 24 : ;
case /* '|' */ 26 : ;
case /* '[' */ 27 : ;
case /* ']' */ 28 : ;
case /* '?' */ 29 : ;
case /* '*' */ 30 : ;
case /* '+' */ 31 : ;
LLtdecr(6);
break;
default:{int LL_12=LLnext(262);
;if (!LL_12) {
LLtdecr(6);
break;
}
else if (LL_12 & 1) goto L_10;}
case /*  C_PARAMS  */ 6 : ;
LLtdecr(6);
LL_SAFE(C_PARAMS);
# line 520 "LLgen.g"
{	if (lextoken.t_num > 15) {
					error(linecount,"Too many parameters", NULL);
				} else	g_setnpar(pres,lextoken.t_num);
				if (g_gettype(pres) == TERMINAL) {
					error(linecount,
						"Terminal with parameters", NULL);
				}
			}
LLread();
}
}
break;
default: if (LLskip()) goto L_8;
goto L_9;
case /*  C_LITERAL  */ 4 : ;
LLsdecr(5);
LL_SAFE(C_LITERAL);
# line 529 "LLgen.g"
{	pe = search(LITERAL,lextoken.t_string,BOTH);
				*pres = *pe;
#ifdef NON_CORRECTING
				if (erroneous)
					pres->g_erroneous = 1;
#endif
			}
LLread();
break;
}
break;
default:
LLtincr(7);
# line 537 "LLgen.g"
{	g_settype(pres,ACTION);
				pres->g_lineno = linecount;
#ifdef NON_CORRECTING
				g_setsubparse(pres, (p_start) 0);
#endif
			}
goto L_11;
L_11 : {switch(LLcsymb) {
default:
break;
case /*  C_SUBSTART  */ 20 : ;
LLtincr(23);
LLtincr(24);
LL_SAFE(C_SUBSTART);
# line 546 "LLgen.g"
{
#ifdef NON_CORRECTING
				nsubstarts++;
#endif
			}
LL_NOSCANDONE(C_IDENT);
# line 553 "LLgen.g"
{
#ifdef NON_CORRECTING
				register p_gram temp;
				register p_start subp;

				temp = search(NONTERM,lextoken.t_string,BOTH);
				subp = (p_start) alloc (sizeof(t_start));

				subp->ff_nont = g_getcont(temp);
				subp->ff_name = (string) 0;
				subp->ff_next = (p_start) 0;
				g_setsubparse(pres, subp);
#endif
			}
LLread();
for (;;) {
goto L_12;
L_12 : {switch(LLcsymb) {
case /* ';' */ 24 : ;
break;
default:{int LL_13=LLnext(44);
;if (!LL_13) {
break;
}
else if (LL_13 & 1) goto L_12;}
case /* ',' */ 23 : ;
LL_SAFE(',');
LL_NOSCANDONE(C_IDENT);
# line 569 "LLgen.g"
{
#ifdef NON_CORRECTING
				register p_gram temp;
				register p_start ff;

				temp = search(NONTERM,lextoken.t_string,BOTH);

				ff = g_getsubparse(pres);
				while (ff) {
					if (ff->ff_nont == g_getcont(temp)) {
						warning(linecount, "\"%s\" used twice in %%substart", lextoken.t_string);
						break;
					}
					ff = ff->ff_next;

				}

				ff = (p_start) alloc(sizeof(t_start));
				ff->ff_nont = g_getcont(temp);
				ff->ff_name = (string) 0;
				ff->ff_next = g_getsubparse(pres);
				g_setsubparse(pres, ff);
#endif
			}
LLread();
continue;
}
}
LLtdecr(23);
break;
}
LLtdecr(24);
LL_SSCANDONE(';');
LLread();
}
}
LLtdecr(7);
LL_SCANDONE(C_ACTION);
LLread();
break;
}
}
static
void
LL8_repeats(
# line 600 "LLgen.g"
int *kind ,int *cnt)  
{
# line 600 "LLgen.g"
	int t1 = 0; 
switch(LLcsymb) {
default:
LL_SAFE('?');
# line 602 "LLgen.g"
{	*kind = OPT; }
LLread();
break;
case /* '*' */ 30 : ;
case /* '+' */ 31 : ;
LLtincr(3);
switch(LLcsymb) {
default:
LL_SAFE('*');
# line 603 "LLgen.g"
{	*kind = STAR; }
break;
case /* '+' */ 31 : ;
LL_SAFE('+');
# line 604 "LLgen.g"
{	*kind = PLUS; }
break;
}
LLread();
goto L_7;
L_7 : {switch(LLcsymb) {
case /*  C_IDENT  */ 2 : ;
case /*  C_LITERAL  */ 4 : ;
case /*  C_ACTION  */ 7 : ;
case /*  C_SUBSTART  */ 20 : ;
case /*  C_ERRONEOUS  */ 21 : ;
case /* ';' */ 24 : ;
case /* '|' */ 26 : ;
case /* '[' */ 27 : ;
case /* ']' */ 28 : ;
LLtdecr(3);
break;
default:{int LL_14=LLnext(259);
;if (!LL_14) {
LLtdecr(3);
break;
}
else if (LL_14 & 1) goto L_7;}
case /*  C_NUMBER  */ 3 : ;
LLtdecr(3);
LL9_number(
# line 606 "LLgen.g"
&t1);
LLread();
}
}
# line 607 "LLgen.g"
{	if (t1 == 1) {
					t1 = 0;
					if (*kind == STAR) *kind = OPT;
					if (*kind == PLUS) *kind = FIXED;
				}
			}
break;
case /*  C_NUMBER  */ 3 : ;
LL9_number(
# line 613 "LLgen.g"
&t1);
LLread();
break;
}
# line 614 "LLgen.g"
{	*cnt = t1; }
}
static
void
LL9_number(
# line 617 "LLgen.g"
int *t)  
{
LL_SAFE(C_NUMBER);
# line 619 "LLgen.g"
{	*t = lextoken.t_num;
				if (*t <= 0 || *t >= 8192) {
					error(linecount,"Illegal number", NULL);
				}
			}
}
static
void
LL4_firsts(
void
) {
# line 626 "LLgen.g"
	register string p; 
LLtincr(23);
LLtincr(2);
LLtincr(24);
LL_SAFE(C_FIRST);
LL_NOSCANDONE(C_IDENT);
# line 628 "LLgen.g"
{	p = store(lextoken.t_string); }
LLtdecr(23);
LL_NOSCANDONE(',');
LLtdecr(2);
LL_NOSCANDONE(C_IDENT);
LLtdecr(24);
LL_NOSCANDONE(';');
# line 630 "LLgen.g"
{	/*
				 * Store this %first in the list belonging
				 * to this input file
				 */
				p_gram temp;
				register p_first ff;

				temp = search(NONTERM,lextoken.t_string,BOTH);
				ff = (p_first) alloc(sizeof(t_first));
				ff->ff_nont = g_getcont(temp);
				ff->ff_name = p;
				ff->ff_next = pfile->f_firsts;
				pfile->f_firsts = ff;
			}
}

# line 645 "LLgen.g"


STATIC p_gram copyrule(register p_gram p,int length)
{
	/*
	 * Returns a pointer to a grammar rule that was created in
	 * p. The space pointed to by p can now be reused
	 */
	register p_gram t;
	p_gram rule;

	t = (p_gram) alloc((unsigned) length * sizeof(t_gram));
	rule = t;
	while (length--) {
		*t++ = *p++;
	}
	return rule;
}

