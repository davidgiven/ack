/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
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
static string	rcsid = "$Header$";
# endif
p_mem		alloc(), new_mem();
string		store();
p_gram		search();

static int	nparams;		/* parameter count for nonterminals */
static int	acount;			/* count #of global actions */
static t_info	term_info,
		link_info;
static p_order	order,
		maxorder;

/* Here are defined : */
extern		a_init();
STATIC p_order	neworder();
STATIC		copyact();
STATIC		mkalt();
STATIC		mkterm();
STATIC p_gram	copyrule();
/* and of course LLparse() */

a_init() {
	term_info.i_esize = sizeof (t_term);
	term_info.i_incr = 50;
	link_info.i_esize = sizeof (t_link);
	link_info.i_incr = 50;
	name_init();
}

STATIC p_order
neworder(index) {
	register p_order po;

	if ((po = order) == maxorder) {
		po = (p_order) alloc(20 * sizeof(*order));
		maxorder = po + 20;
	}
	order = po + 1;
	po->o_next = 0;
	po->o_index = index;
	if (porder) {
		porder->o_next = po;
	}
	else	sorder = po;
	return po;
}
}

%start	LLparse, spec;

spec	:		{	acount = 0; }
	  [ %persistent def ]*
			{	/*
				 * Put an endmarker in temporary file
				 */
				fprintf(fact,"%c%c",'\0', '\0');
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
			{ 	p = store(lextoken.t_string); }
	  ',' C_IDENT ';'
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
				ff->ff_nont = g_getnont(temp);
				ff->ff_name = p;
				ff->ff_next = start;
				start = ff;
			}
	| C_LEXICAL C_IDENT
			{	if (!lexical) {
					lexical = store(lextoken.t_string);
				}
				else	error(linecount,"Duplicate %%lexical");
			}
	  ';'
	  /*
	   * Declaration of a name for the lexical analyser.
	   * May appear only once
	   */
	| action(0)	{	acount++; }
	  /*
	   * A global C-declaration
	   */
	| firsts
	  /*
	   * declarations for macros
	   */
	;

listel	: C_IDENT	{	search(TERMINAL,lextoken.t_string,ENTERING); }
	;

rule			{	register p_nont p;
				p_gram rr;
				register p_gram temp;
			}
	: /*
	   * grammar for a production rule
	   */
	  C_IDENT	{	temp = search(NONTERM,lextoken.t_string,BOTH);
	 			p = &nonterms[g_getnont(temp)];
				if (p->n_rule) {
					error(linecount,
"nonterminal %s already defined", lextoken.t_string);
				}
				/*
				 * Remember the order in which the nonterminals
				 * were defined. Code must be generated in that
				 * order to keep track with the actions on the
				 * temporary file
				 */
				porder = neworder(p - nonterms);
				p->n_count = acount;
				acount = 0;
				p->n_lineno = linecount;
	  		}
	  [ params(2)	{	p->n_flags |= PARAMS;
				if (nparams > 15) {
					error(linecount,"Too many parameters");
				}
				else	setntparams(p,nparams);
			}
	  ]?
	  [ action(0)	{	p->n_flags |= LOCALS; }
	  ]?
	  ':' productions(&rr) ';'
			/*
			 * Do not use p->n_rule now! The nonterms array
			 * might have been re-allocated.
			 */
	  		{	nonterms[g_getnont(temp)].n_rule = rr;}
	;

action(int n;)
	/*
	 * The parameter n is non-zero when the opening and closing
	 * bracket must be copied along with the action
	 */
	: '{'		{	copyact('{','}',n,0); }
	  '}'
	;

productions(p_gram *p;)
	/*
	 * One or more alternatives
	 */
	{	p_gram		prod;
		int		conflres = 0;
		int		t = 0;
		int		haddefault = 0;
		t_gram		alts[100];
		register p_gram	p_alts = alts;
		int		o_lc, n_lc;
	} :
			{	o_lc = linecount; }
	  simpleproduction(p,&conflres)
			{	if (conflres & DEF) haddefault = 1; }
	  [ 
	    [ '|'	{	n_lc = linecount; }
	      simpleproduction(&prod,&t)
			{	if (p_alts - alts >= 97) {
					error(n_lc,"Too many alternatives");
					p_alts = alts;
				}
				if (t & DEF) {
					if (haddefault) {
						error(n_lc,
		"More than one %%default in alternation");
					}
					haddefault = 1;
				}
				mkalt(*p,conflres,o_lc,p_alts++);
				o_lc = n_lc;
				conflres = t;
				t = 0;
				*p = prod;
			}
	    ]+		{	if (conflres & ~DEF) {
					error(n_lc,
		"Resolver on last alternative not allowed");
				}
	  			mkalt(*p,conflres,n_lc,p_alts++);
				g_settype(p_alts,EORULE);
				*p = copyrule(alts,p_alts+1-alts);
	  		}
	  |
			{	if (conflres & ~DEF) {
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
	;
{

STATIC
mkalt(prod,condition,lc,res) p_gram prod; register p_gram res; {
	/*
	 * Create an alternation and initialise it.
	 */
	register int		hulp;
	register p_link		l;

	l = (p_link) new_mem(&link_info);
	links = (p_link) link_info.i_ptr;
	hulp = l - links;
	l->l_rule = prod;
	l->l_flag = condition;
	g_setcont(res,hulp);
	g_settype(res,ALTERNATION);
	res->g_lineno = lc;
}
}

simpleproduction(p_gram *p; register int *conflres;)
	{	t_gram		rule[100];
		t_gram		elem;
		register p_gram	p_rule = rule;
		int		cnt, kind;
	} :
	  [ C_DEFAULT	{	*conflres = DEF; }
	  ]?
	  [
	    /*
	     * Optional conflict reslover
	     */
	      C_IF expr	{	*conflres |= COND; }
	    | C_PREFER	{	*conflres |= PREFERING; }
	    | C_AVOID	{	*conflres |= AVOIDING; }
	  ]?
	  [ %persistent elem(&elem)
	 		{	if (p_rule - rule >= 98) {
					error(linecount,"Production too long");
					p_rule = rule;
				}
				kind = FIXED;
				cnt = 0;
			}
	    [ repeats(&kind, &cnt)
			{	if (g_gettype(&elem) != TERM) {
					*p_rule = elem;
					g_settype(p_rule+1,EORULE);
					mkterm(copyrule(p_rule,2),
					       0,
					       p_rule->g_lineno,
					       &elem);
				}
			}
	    ]?		{	if (g_gettype(&elem) == TERM) {
					register p_term q;

					q = &terms[g_getcont(&elem)];
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
							"illegal %%persistent");
					}
					*/
				}
				*p_rule++ = elem;
			}
	  ]*		{	register p_term q;
	  
				g_settype(p_rule,EORULE);
				*p = 0;
				if (g_gettype(&rule[0]) == TERM &&
				    p_rule-rule == 1) {
				    	q = &terms[g_getcont(&rule[0])];
					if (r_getkind(q) == FIXED &&
					    r_getnum(q) == 0) {
					    	*p = q->t_rule;
					}
				}
				if (!*p) *p = copyrule(rule,p_rule-rule+1);
	  		}
	;
{

STATIC
mkterm(prod,flags,lc, result) p_gram prod; register p_gram result; {
	/*
	 * Create a term, initialise it and return
	 * a grammar element containing it
	 */
	register p_term		q;
	unsigned		hulp;
	
	q = (p_term) new_mem(&term_info);
	terms = (p_term) term_info.i_ptr;
	hulp = q - terms;
	q->t_rule = prod;
	q->t_contains = 0;
	q->t_flags = flags;
	g_settype(result,TERM);
	g_setcont(result,hulp);
	result->g_lineno = lc;
}
}

elem (register p_gram pres;)
	{	register short	t = 0;
		p_gram		p1;
		int		ln;
		p_gram		pe;
	} :
	  '['		{	ln = linecount; }
	  [ C_WHILE expr	{	t |= RESOLVER; }
	  ]?
	  [ C_PERSISTENT	{	t |= PERSISTENT; }
	  ]?
	  productions(&p1)
	  ']'		{
	  			mkterm(p1,t,ln,pres);
			}
	|
	  C_IDENT	{	pe = search(UNKNOWN,lextoken.t_string,BOTH);
				*pres = *pe;
	 		}
	  [ params(0)	{	if (nparams > 14) {
					error(linecount,"Too many parameters");
				} else	g_setnpar(pres,nparams+1);
				if (g_gettype(pres) == TERMINAL) {
					error(linecount,
						"Terminal with parameters");
				}
			}
	  ]?
	| C_LITERAL	{	pe = search(LITERAL,lextoken.t_string,BOTH);
				*pres = *pe;
			}
	|		{	g_settype(pres,ACTION);
				pres->g_lineno = linecount;
			}
	  action(1)
	;

params(int n;)
	: '('		{	copyact('(',')',n,0); }
	  ')'
	;

expr	: '('		{	copyact('(',')',1,0); }
	  ')'
	;

repeats(int *kind, *cnt;)	{	int t1 = 0; } :
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
			{ 	p = store(lextoken.t_string); }
	  ',' C_IDENT ';'
			{	/*
				 * Store this %first in the list belonging
				 * to this input file
				 */
				p_gram temp;
				register p_first ff;

				temp = search(NONTERM,lextoken.t_string,BOTH);
				ff = (p_first) alloc(sizeof(t_first));
				ff->ff_nont = g_getnont(temp);
				ff->ff_name = p;
				ff->ff_next = pfile->f_firsts;
				pfile->f_firsts = ff;
			}
	;
{

STATIC
copyact(ch1,ch2,flag,level) char ch1,ch2; {
	/*
	 * Copy an action to file f. Opening bracket is ch1, closing bracket
	 * is ch2.
	 * If flag = 1, copy opening and closing parameters too.
	 * If flag = 2, the copy is a parameter declaration copy. 
	 * Give an error message if not ending on a ';'
	 */
	register	FILE *f;
	register	ch;		/* Current char */
	register	match;		/* used to read strings */
	int		saved;		/* save linecount */
	int		semicolon = 0;

	f = fact;
	if (!level) {
		saved = linecount;
		nparams = 0;			/* count comma's */
		fprintf(f,"%c# line %d \"%s\"\n", '\0', linecount,f_input);
	}
	if (level || flag == 1) putc(ch1,f);
	for (;;) {
		ch = input();
		if (ch == ch2) {
			if (!level) unput(ch);
			if (level || flag == 1) putc(ch,f);
			if ((!level) && flag == 2 && !semicolon) {
				error(linecount,"missing ';'");
			}
			return;
		}
		if (c_class[ch] != ISSPA) semicolon = 0;
		switch(ch) {
		  case ')':
		  case '}':
		  case ']':
			error(linecount,"Parentheses mismatch");
			break;
		  case '(':
			copyact('(',')',flag,level+1);
			continue;
		  case '{':
			copyact('{','}',flag,level+1);
			continue;
		  case '[':
			copyact('[',']',flag,level+1);
			continue;
		  case '/':
			ch = input();
			unput(ch);
			if (ch == '*') {
				skipcomment(1);
				continue;
			}
			ch = '/';
			break;
		  case ';':
			semicolon = 1;
			/* Fall through */
		  case ',':
			if (!level) {		/*
						 * Only ','s and ';'s on the
						 * outer level are counted
						 */
				nparams++;
			}
			break;
		  case '\'':
		  case '"' :
			/* 
			 * watch out for brackets in strings, they do not 
			 * count !
			 */
			match = ch;
			putc(ch,f);
			while((ch = input())) {
				if (ch == match) break;
				if (ch == '\\') {
					putc(ch,f);
					ch = input();
				}
				if (ch == '\n') {
					error(linecount,"newline in string");
					unput(match);
				}
				putc(ch,f);
			}
			if (ch == match) break;
			/* Fall through */
		    case EOF :
			if (!level) error(saved,"Action does not terminate");
			return;
		}
		putc(ch,f);
	}
}

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
