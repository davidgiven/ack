/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * For full copyright and restrictions on use see the file COPYING in the top
 * level of the LLgen tree.
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * LLgen.g
 * Defines the grammar of LLgen.
 * Some routines that build the internal structure are also included
 */

{
# include "types.h"
# include "io.h"
# include "extern.h"
# include "assert.h"
# include "cclass.h"

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
STATIC		newnorder();
STATIC		newtorder();
STATIC		mkalt();
STATIC		mkterm();
STATIC p_gram	copyrule();
/* and of course LLparse() */

STATIC
newnorder(index) {
	static int porder;

	if (norder != -1) {
		nonterms[porder].n_next = index;
	}
	else	norder = index;
	porder = index;
	nonterms[porder].n_next = -1;
}

STATIC
newtorder(index) {
	static int porder;

	if (torder != -1) {
		tokens[porder].t_next = index;
	}
	else	torder = index;
	porder = index;
	tokens[porder].t_next = -1;
}

p_init()
{
	alt_table = (p_gram )alloc(ALTINCR*sizeof(t_gram));
	n_alts = 0;
	max_alts = ALTINCR;
	rule_table = (p_gram )alloc(RULEINCR*sizeof(t_gram));
	n_rules = 0;
	max_rules = RULEINCR;
}

}

%start	LLparse, spec;

spec	:		{	acount = 0; p_init(); }
	  [ %persistent def ]*
			{	/*
				 * Put an endmarker in temporary file
				 */
				putc('\0', fact);
				putc('\0', fact);
				free((p_mem) rule_table);
				free((p_mem) alt_table);
			}
	;

def			{	register string p; }
	: rule
	  /*
	   * A grammar rule
	   */
	| C_TOKEN listel [ ',' listel ]* ';'
	  /*
	   * A token declaration
	   */
	| C_START C_IDENT
			{	p = store(lextoken.t_string); }
	  ',' C_IDENT
	  /*
	   * A start symbol declaration
	   */
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
	  ';'
	| C_LEXICAL C_IDENT
	  /*
	   * Declaration of a name for the lexical analyser.
	   * May appear only once
	   */
			{	if (!lexical) {
					lexical = store(lextoken.t_string);
				}
				else	error(linecount,"Duplicate %%lexical");
			}
	  ';'
	| C_PREFIX C_IDENT
	  /*
	   * Prefix of external names (default: LL)
	   */
			{	if (!prefix) {
					prefix = store(lextoken.t_string);
					if (strlen(prefix) > 6) {
						error(linecount,
							"%%prefix too long");
						prefix[6] = 0;
					}
				}
				else	error(linecount,"Duplicate %%prefix");
			}
	  ';'
	| C_ONERROR C_IDENT
			{
#ifdef NON_CORRECTING
				if (non_corr) {
					warning(linecount, "%%onerror conflicts with -n option");
				}
				else
#endif
				  if (! onerror) {
					onerror = store(lextoken.t_string);
				}
				else	error(linecount,"Duplicate %%onerror");
			}
	  ';'
	| C_ACTION	{	acount++; }
	  /*
	   * A global C-declaration
	   */
	| firsts
	  /*
	   * declarations for macros
	   */
	;

listel	: C_IDENT	{	p_gram temp = search(TERMINAL,lextoken.t_string,ENTERING);
				newtorder(g_getcont(temp));
				tokens[g_getcont(temp)].t_lineno = linecount;
			}
	;

rule			{	register p_nont p;
				p_gram rr;
				register p_gram temp;
			}
	: /*
	   * grammar for a production rule
	   */
	  C_IDENT	{	temp = search(NONTERM,lextoken.t_string,BOTH);
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
	  [ C_PARAMS	{	if (lextoken.t_num > 0) {
					p->n_flags |= PARAMS;
					if (lextoken.t_num > 15) {
						error(linecount,"Too many parameters");
					}
					else	setntparams(p,lextoken.t_num);
				}
			}
	  ]?
	  [ C_ACTION	{	p->n_flags |= LOCALS; }
	  ]?
	  ':' 		{ 	in_production = 1; }
	  productions(&rr) ';'
			{	in_production = 0; }
			/*
			 * Do not use p->n_rule now! The nonterms array
			 * might have been re-allocated.
			 */
			{	nonterms[g_getcont(temp)].n_rule = rr;}
	;

productions(p_gram *p;)
	/*
	 * One or more alternatives
	 */
	{	p_gram		prod;
		int		conflres = 0;
		int		t = 0;
		int		haddefault = 0;
		int		altcnt = 0;
		int		o_lc, n_lc;
	} :
			{	o_lc = linecount; }
	  simpleproduction(p,&conflres)
			{	if (conflres & DEF) haddefault = 1; }
	  [
	    [ '|'	{	n_lc = linecount; }
	      simpleproduction(&prod,&t)
			{	if (n_alts >= max_alts-2) {
					alt_table = (p_gram ) ralloc(
						(p_mem) alt_table,
						(unsigned)(max_alts+=ALTINCR)*sizeof(t_gram));
				}
				if (t & DEF) {
					if (haddefault) {
						error(n_lc,
		"More than one %%default in alternation");
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
	    ]+		{	if (conflres & (COND|PREFERING|AVOIDING)) {
					error(n_lc,
		"Resolver on last alternative not allowed");
				}
				mkalt(*p,conflres,n_lc,&alt_table[n_alts++]);
				altcnt++;
				g_settype((&alt_table[n_alts]),EORULE);
				*p = copyrule(&alt_table[n_alts-altcnt],altcnt+1);
			}
	  |
			{	if (conflres & (COND|PREFERING|AVOIDING)) {
					error(o_lc,
		"No alternation conflict resolver allowed here");
				}
				/*
				if (conflres & DEF) {
					error(o_lc,
		"No %%default allowed here");
				}
				*/
			}
	  ]
			{	n_alts -= altcnt; }
	;
{

STATIC
mkalt(prod,condition,lc,res) p_gram prod; register p_gram res; {
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
}

simpleproduction(p_gram *p; register int *conflres;)
	{	t_gram		elem;
		int		elmcnt = 0;
		int		cnt, kind;
		int		termdeleted = 0;
	} :
	  [ C_DEFAULT	{	*conflres |= DEF; }
	  ]?
	  [
	    /*
	     * Optional conflict reslover
	     */
	      C_IF C_EXPR {	*conflres |= COND; }
	    | C_PREFER	{	*conflres |= PREFERING; }
	    | C_AVOID	{	*conflres |= AVOIDING; }
	  ]?
	  [ C_ILLEGAL	{
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
					error(linecount, "%%illegal not allowed in %%default rule");
				}
#endif
			}
	  ]?
	  [ %persistent elem(&elem)
			{	if (n_rules >= max_rules-2) {
					rule_table = (p_gram) ralloc(
						  (p_mem) rule_table,
						  (unsigned)(max_rules+=RULEINCR)*sizeof(t_gram));
				}
				kind = FIXED;
				cnt = 0;
			}
	    [ repeats(&kind, &cnt)
			{	if (g_gettype(&elem) != TERM) {
					rule_table[n_rules] = elem;
					g_settype((&rule_table[n_rules+1]),EORULE);
					mkterm(copyrule(&rule_table[n_rules],2),
					       0,
					       elem.g_lineno,
					       &elem);
				}
			}
	    |
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
	    ]		{	if (!termdeleted && g_gettype(&elem) == TERM) {
					register p_term q;

					q = g_getterm(&elem);
					r_setkind(q,kind);
					r_setnum(q,cnt);
					if ((q->t_flags & RESOLVER) &&
					    (kind == PLUS || kind == FIXED)) {
						error(linecount,
		"%%while not allowed in this term");
					}
					/*
					 * A persistent fixed term is the same
					 * as a non-persistent fixed term.
					 * Should we complain?
					if ((q->t_flags & PERSISTENT) &&
					    kind == FIXED) {
						error(linecount,
							"Illegal %%persistent");
					}
					*/
				}
				termdeleted = 0;
				elmcnt++;
				rule_table[n_rules++] = elem;
			}
	  ]*		{	register p_term q;

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
	;
{

STATIC
mkterm(prod,flags,lc,result) p_gram prod; register p_gram result; {
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
}

elem (register p_gram pres;)
	{	register int	t = 0;
		p_gram		p1;
		int		ln;
		p_gram		pe;
#ifdef NON_CORRECTING
		int		erroneous = 0;
#endif
	} :
	  '['		{	ln = linecount; }
	  [ C_WHILE C_EXPR	{	t |= RESOLVER; }
	  ]?
	  [ C_PERSISTENT	{	t |= PERSISTENT; }
	  ]?
	  productions(&p1)
	  ']'		{
				mkterm(p1,t,ln,pres);
			}
	|
	  [ C_ERRONEOUS		{
#ifdef NON_CORRECTING
					erroneous = 1;
#endif
				}
	  ]?

	  [
	  C_IDENT	{	pe = search(UNKNOWN,lextoken.t_string,BOTH);
				*pres = *pe;
#ifdef NON_CORRECTING
				if (erroneous) {
					if (g_gettype(pres) != TERMINAL){
						warning(linecount,
							"Erroneous only allowed on terminal");
						erroneous = 0;
					}
					else
						pres->g_erroneous = 1;
				}
#endif

			}
	  [ C_PARAMS	{	if (lextoken.t_num > 15) {
					error(linecount,"Too many parameters");
				} else	g_setnpar(pres,lextoken.t_num);
				if (g_gettype(pres) == TERMINAL) {
					error(linecount,
						"Terminal with parameters");
				}
			}
	  ]?
	| C_LITERAL	{	pe = search(LITERAL,lextoken.t_string,BOTH);
				*pres = *pe;
#ifdef NON_CORRECTING
				if (erroneous)
					pres->g_erroneous = 1;
#endif
			}
	  ]
	|		{	g_settype(pres,ACTION);
				pres->g_lineno = linecount;
#ifdef NON_CORRECTING
				g_setsubparse(pres, (p_start) 0);
#endif
			}

	  [ C_SUBSTART

			{
#ifdef NON_CORRECTING
				nsubstarts++;
#endif
			}

	    C_IDENT
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

		[ ',' C_IDENT
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

	       ]* ';'
	   ]?

	  C_ACTION
	;

repeats(int *kind; int *cnt;)	{	int t1 = 0; } :
	  [
	    '?'		{	*kind = OPT; }
	  | [ '*'	{	*kind = STAR; }
	    | '+'	{	*kind = PLUS; }
	    ]
	    number(&t1)?
			{	if (t1 == 1) {
					t1 = 0;
					if (*kind == STAR) *kind = OPT;
					if (*kind == PLUS) *kind = FIXED;
				}
			}
	  | number(&t1)
	  ]		{	*cnt = t1; }
	;

number(int *t;)
	: C_NUMBER
			{	*t = lextoken.t_num;
				if (*t <= 0 || *t >= 8192) {
					error(linecount,"Illegal number");
				}
			}
	;

firsts	{	register string p; }
	: C_FIRST C_IDENT
			{	p = store(lextoken.t_string); }
	  ',' C_IDENT ';'
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
	;
{

STATIC p_gram
copyrule(p,length) register p_gram p; {
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
}
