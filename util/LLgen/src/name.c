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
 * name.c
 * Defines the symboltable search routine and an initialising routine
 */

# include "types.h"
# include "tunable.h"
# include "extern.h"
# include "assert.h"
# include "io.h"

# ifndef NORCSID
static string rcsid7 = "$Header$";
# endif

# define HASHSIZE 128

static char	name[NAMESZ];			/* space for names */
static int	iname;				/* index in nametable */
static p_entry	h_root[HASHSIZE];		/* hash table */
static string	e_literal = "Illegal literal";

/* Defined in this file are: */
extern string	store();
extern		name_init();
STATIC int	hash();
extern t_gram	search();

string	
store(s) register string s; {
	/*
	 * Store a string s in the name table
	 */
	register string	t,u;

	u = t = &name[iname];
	do {	if (u > &name[NAMESZ-1]) fatal(linecount,"name table overflow");
		else *u++ = *s;
	} while (*s++);
	iname = u - name;
	return t;
}

name_init() {
	/*
	 * Initialise hash-table and enter special terminal EOFILE
	 */
	register p_entry	*p;
	t_gram			search();

	for(p = h_root; p<= &h_root[HASHSIZE-1]; p++) *p = 0;
	search(TERMINAL,"EOFILE",ENTERING);
}

STATIC int
hash(str) string str; {
	/*
	 * Compute the hash for string str
	 */
	register	i;
	register string l;

	l = str;
	i = 0;
	while (*l != '\0') i += *l++ & 0377;
	i += l - str;
	return i % HASHSIZE;
}

t_gram
search(type,str,option) register string str; {
	/*
	 * Search for object str.
	 * It has type UNKNOWN, LITERAL, TERMINAL or NONTERM.
	 * option can be ENTERING, JUSTLOOKING or BOTH.
	 */
	register int		val;
	register p_entry	p;
	t_gram			r;
	register int		i;

	g_init(&r);
	g_setcont(&r,UNDEFINED);
	r.g_lineno = linecount;
	i = hash(str);
	/*
	 * Walk hash chain
	 */
	for (p = h_root[i]; p != (p_entry) 0; p = p->h_next) {
		if(!strcmp(p->h_name,str)) {
			val = p - h_entry;
			if (type == LITERAL &&
			    (val >= NTERMINALS || p->h_num >= 0400)) continue;
			if (val>=NTERMINALS) {
				/* Should be a nonterminal */
				if (type == TERMINAL) {
					error(linecount,
						"%s : terminal expected",
						str);
				}
				g_settype(&r,NONTERM);
				g_setnont(&r,val - NTERMINALS);
			} else {
				if (type != LITERAL && p->h_num < 0400) {
					continue;
				}
				if (type == NONTERM) {
					error(linecount,
						"%s : nonterminal expected",
						str);
					continue;
				}
				g_setnont(&r, val);
				g_settype(&r, TERMINAL);
			}
			if (option==ENTERING)  {
				error(linecount,
					"%s : already defined",str);
			}
			return r;
		}
	}
	if (option == JUSTLOOKING) return r;
	if (type == TERMINAL || type == LITERAL) {
		if (nterminals == NTERMINALS) {
			fatal(linecount,"too many terminals");
		}
		p = &h_entry[nterminals];
	} else {
		/*
		 * type == NONTERM || type == UNKNOWN 
		 * UNKNOWN and not yet declared means : NONTERM
		 */
		if (nnonterms == NNONTERMS) {
			fatal(linecount,"too many nonterminals");
		}
		p = &h_entry[NTERMINALS+nnonterms];
	}
	p->h_name = store(str);
	p->h_next = h_root[i];
	h_root[i] = p;
	if (type == NONTERM || type == UNKNOWN) {
		register p_nont q;

		q = &nonterms[nnonterms];
		q->n_rule = 0;
		q->n_string = f_input;
		q->n_follow = 0;
		q->n_flags = 0;
		q->n_contains = 0;
		p->h_num = 0;
		g_settype(&r, NONTERM);
		g_setnont(&r, nnonterms);
		nnonterms++;
		return r;
	}
	if (type == LITERAL) {
		if (str[0] == '\\') {
			/*
			 * Handle escapes in literals
			 */
			if (str[2] == '\0') {
				switch(str[1]) {
				  case 'n' :
				  	val = '\n';
					break;
				  case 'r' :
					val = '\r';
					break;
				  case 'b' :
					val = '\b';
					break;
				  case 'f' :
				 	val = '\f';
					break;
				  case 't' :
				  	val = '\t';
					break;
				  case '\'':
				  	val = '\'';
					break;
				  case '\\':
				  	val = '\\';
					break;
				  default  :
				  	error(linecount,e_literal);
				}
			} else {
				/*
				 * Here, str[2] != '\0'
				 */
				if (str[1] > '3' || str[1] < '0' ||
				    str[2] > '7' || str[2] < '0' ||
				    str[3] > '7' || str[3] < '0' ||
				    str[4] != '\0') error(linecount,e_literal);
				val = 64*str[1] - 73*'0' + 8*str[2] + str[3];
			}
		} else { 
			/*
			 * No escape in literal
			 */
			if (str[1] == '\0') val = str[0];
			else error(linecount,e_literal);
		}
		p->h_num = val;
	} else {
		/*
		 * Here, type = TERMINAL
		 */
		p->h_num = assval++;
	}
	g_settype(&r, TERMINAL);
	g_setnont(&r, nterminals);
	nterminals++;
	return r;
}
