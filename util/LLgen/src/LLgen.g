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
 * Some routines that are included build the internal structure
 */

{
# include "types.h"
# include "io.h"
# include "tunable.h"
# include "extern.h"
# include "assert.h"

# ifndef NORCSID
static string	rcsidb = "$Header$";
# endif
p_mem		alloc();
string		store();
t_gram		search();

static int	nparams;		/* parameter count for nonterminals */
static t_gram	elem;			/* temporary space */
static int	acount;			/* count #of global actions */

/* Here are defined : */
STATIC		copyact();
STATIC unsigned	get();
STATIC t_gram	mkalt();
STATIC t_gram	mkterm();
STATIC p_gram	copyrule();
}

%start	LLparse, spec;

spec	:		{	acount = 0; }
	  [ %persistent def ]*
			{	/*
				 * Put an endmarker in temporary file
				 */
				putc('\0',fact); putc('\0',fact);
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
				t_gram temp;
				register p_start ff;

				temp = search(NONTERM,lextoken.t_string,BOTH);
				ff = (p_start) alloc(sizeof(t_start));
				ff->ff_nont = &nonterms[g_getnont(&temp)];
				ff->ff_name = p;
				ff->ff_next = start;
				start = ff;
			}
	| C_LEXICAL C_IDENT
			{	if (!lexical) {
					lexical = store(lextoken.t_string);
				} else	error(linecount,"Duplicate %%lexical");
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

rule			{	register p_nont p;}
	: /*
	   * grammar for a production rule
	   */
	  C_IDENT	{	t_gram temp;

				temp = search(NONTERM,lextoken.t_string,BOTH);
	 			p = &nonterms[g_getnont(&temp)];
				if (p->n_rule) {
					error(linecount,
						"nonterminal already defined");
				}
				/*
				 * Remember the order in which the nonterminals
				 * were defined. Code must be generated in that
				 * order to keep track with the actions on the
				 * temporary file
				 */
				*maxorder++ = p - nonterms;
				p->n_count = acount;
				acount = 0;
				p->n_lineno = linecount;
	  		}
	  [ params(2)	{	p->n_flags |= PARAMS;
				if (nparams > 7) {
					error(linecount,"Too many parameters");
				} else	p->n_flags |= (nparams << 3);
			}
	  ]?
	  [ action(0)	{	p->n_flags |= LOCALS; }
	  ]?
	  ':' productions(&(p->n_rule)) ';'
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
			{	n_lc = o_lc = linecount; }
	  simpleproduction(p,&conflres)
	  [ '|'		{	n_lc = linecount; }
	    [ C_DEFAULT	{	if (haddefault) {
					error(linecount,
						"multiple %%default");
				}
				haddefault = 1;
				t |= DEF;
			}
	    ]?
	    simpleproduction(&prod,&t)
			{	if (p_alts - alts >= 97) {
					error(n_lc,"Too many alternatives");
					p_alts = alts;
				}
				*p_alts++ = mkalt(*p,conflres,o_lc);
				o_lc = n_lc;
				conflres = t;
				t = 0;
				*p = prod;
			}
	  ]*		{	if (conflres & ~DEF) {
					error(n_lc,
						"Resolver on last alternative");
				}
				if (p_alts > alts) {
	  				*p_alts++ = mkalt(*p,conflres,n_lc);
					g_settype(p_alts,EORULE);
					*p = copyrule(alts,p_alts+1-alts);
					if (!haddefault) {
						((p_link) pentry[g_getcont(*p)])
							->l_flag |= DEF;
					}
				}
	  		}
	;
{

STATIC t_gram
mkalt(prod,condition,lc) p_gram prod; {
	/*
	 * Create an alternation, initialise it and return
	 * a grammar element containing it
	 */
	register unsigned	hulp;
	register p_link		l;
	t_gram			r;

	g_init(&r);
	hulp = get(sizeof(*l));
	l = (p_link) pentry[hulp];
	l->l_rule = prod;
	l->l_flag = condition;
	g_setcont(&r,hulp);
	g_settype(&r,ALTERNATION);
	r.g_lineno = lc;
	return r;
}
}

simpleproduction(p_gram *p; register int *conflres;)
	{	t_gram		rule[100];
		register p_gram	p_rule = rule;
		t_reps		reps;
	} :
			{	r_init(&reps); }
	  [
	    /*
	     * Optional conflict reslover
	     */
	      C_IF expr	{	*conflres |= COND; }
	    | C_PREFER	{	*conflres |= PREFERING; }
	    | C_AVOID	{	*conflres |= AVOIDING; }
	  ]?
	  [ %persistent elem
	 		{	if (p_rule - rule >= 98) {
					error(linecount,"Production too long");
					p_rule = rule;
				}
				r_setkind(&reps,FIXED);
				r_setnum(&reps,0);
			}
	    [ repeats(&reps)
			{	if (g_gettype(&elem) != TERM) {
					*p_rule = elem;
					g_settype(p_rule+1,EORULE);
					elem = mkterm(copyrule(p_rule,2),
							0,
							&reps,
							p_rule->g_lineno);
				}
			}
	    ]?		{	if (g_gettype(&elem) == TERM) {
					register p_term q;

					q = (p_term) pentry[g_getcont(&elem)];
					q->t_reps = reps;
					if (q->t_flags & RESOLVER &&
					    (r_getkind(&reps) == PLUS ||
					     r_getkind(&reps) == FIXED)) {
						error(linecount,
							"illegal %%while");
					}
					if (q->t_flags & PERSISTENT &&
					    r_getkind(&reps) == FIXED) {
						error(linecount,
							"illegal %%persistent");
					}
				}
				*p_rule++ = elem;
			}
	  ]*		{	register p_term q;
	  
				g_settype(p_rule,EORULE);
				if (g_gettype(&rule[0]) == TERM &&
				    p_rule-rule == 1) {
				    	q=(p_term)pentry[g_getcont(&rule[0])];
					if (r_getkind(&(q->t_reps))==FIXED &&
					    r_getnum(&(q->t_reps)) == 0) {
					    	*p = q->t_rule;
					}
					else *p = copyrule(rule,2);
				}
				else *p = copyrule(rule,p_rule-rule+1);
	  		}
	;
{

STATIC t_gram
mkterm(prod,flags,reps,lc) p_gram prod; register p_reps reps; {
	/*
	 * Create a term, initialise it and return
	 * a grammar element contianing it
	 */
	register p_term		q;
	register unsigned	hulp;
	t_gram			r;
	
	g_init(&r);
	hulp = get(sizeof(*q));
	q = (p_term) pentry[hulp];
	q->t_rule = prod;
	q->t_contains = 0;
	/* "*1" = "?" */
	if (r_getnum(reps) == 1 && r_getkind(reps) == STAR) {
		r_setkind(reps,OPT);
	}
	q->t_reps = *reps;
	q->t_flags = flags;
	g_settype(&r,TERM);
	g_setcont(&r,hulp);
	r.g_lineno = lc;
	return r;
}
}

elem	{	register short	t = 0;
		p_gram		p1;
		t_reps		reps;
		int		ln;
	} :
	  '['		{	ln = linecount; }
	  [ C_WHILE expr	{	t |= RESOLVER; }
	  ]?
	  [ C_PERSISTENT	{	t |= PERSISTENT; }
	  ]?
	  productions(&p1)
	  ']'		{	r_init(&reps);
	  			elem = mkterm(p1,t,&reps,ln);
			}
	| %default
	  C_IDENT	{	elem = search(UNKNOWN,lextoken.t_string,BOTH); }
	  [ params(0)	{	if (nparams > 6) {
					error(linecount,"Too many parameters");
				} else	g_setnpar(&elem,nparams+1);
				if (g_gettype(&elem) == TERMINAL) {
					error(linecount,
						"Terminal with parameters");
				}
			}
	  ]?
	| C_LITERAL	{	elem = search(LITERAL,lextoken.t_string,BOTH); }
	|		{	g_settype(&elem,ACTION);
				elem.g_lineno = linecount;
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

repeats(t_reps *t;)	{	int t1 = 0; } :
	  [
	    '?'		{	r_setkind(t,OPT); }
	  | [ '*'	{	r_setkind(t,STAR); }
	    | '+'	{	r_setkind(t,PLUS); }
	    ]
	    number(&t1)?
			{	if (t1 == 1 && r_getkind(t) == PLUS) {
					error(linecount,
						"Illegal repetition specifier");
				}
			}
	  | number(&t1)
	  ]		{	r_setnum(t,t1); }
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
				t_gram temp;
				register p_first ff;

				temp = search(NONTERM,lextoken.t_string,BOTH);
				ff = (p_first) alloc(sizeof(t_first));
				ff->ff_nont = &nonterms[g_getnont(&temp)];
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
		putc('\0',f);
		fprintf(f,"# line %d \"%s\"\n", linecount,f_input);
		if (flag == 1) putc(ch1,f);
	}
	else	putc(ch1,f);
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
		if (! isspace(ch)) semicolon = 0;
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

static int	ecount;			/* Index in "pentry" array */
STATIC unsigned
get(size) unsigned size; {
	/*
	 * Get some core, save a pointer to it in "pentry",
	 * return index in pentry
	 */

	if (ecount >= ENTSIZ) fatal(linecount,"Entry table overflow");
	pentry[ecount] = alloc(size);
	return ecount++;
}

STATIC p_gram
copyrule(p,length) register p_gram p; register length; {
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
