/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * All rights reserved.
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
 * Defines the symboltable search routine, a name store routine and an
 * initialising routine.
 */

# include "types.h"
# include "extern.h"
# include "assert.h"
# include "io.h"

# ifndef NORCSID
static string rcsid7 = "$Header$";
# endif

# define HASHSIZE 128
# define NMSIZ 2048	/* Room for names allocated NMSIZ bytes at a time */

static char	*name, *maxname;
static p_entry	h_root[HASHSIZE];		/* hash table */
static string	e_literal = "Illegal literal";
static p_entry	entries, maxentries;
static t_info	token_info, nont_info;

/* Defined in this file are: */
extern string	store();
extern		name_init();
STATIC int	hash();
STATIC p_entry	newentry();
extern p_gram	search();

p_mem alloc();
p_mem new_mem();

name_init() {
	token_info.i_esize = sizeof (t_token);
	token_info.i_incr = 50;
	nont_info.i_esize = sizeof (t_nont);
	nont_info.i_incr = 50;
	search(TERMINAL,"EOFILE",ENTERING);
}

STATIC p_entry
newentry(str, next) string str; p_entry next; {
	register p_entry p;

	if ((p = entries) == maxentries) {
		p = (p_entry) alloc(50 * sizeof(t_entry));
		maxentries = p + 50;
	}
	entries = p + 1;
	p->h_name = str;
	p->h_next = next;
	p->h_type.g_lineno = linecount;
	return p;
}

string	
store(s) string s; {
	/*
	 * Store a string s in the name table
	 */
	register string	s1, t ,u;

	u = name;
	t = s;
	s1 = u;
	do {
		if (u >= maxname) {
			u = alloc(NMSIZ);
			maxname = u + NMSIZ;
			t = s;
			s1 = u;
		}
		*u++ = *t;
	} while (*t++);
	name = u;
	return s1;
}

STATIC int
hash(str) string str; {
	/*
	 * Compute the hash for string str
	 */
	register int	i;
	register string l;

	l = str;
	i = 0;
	while (*l != '\0') i += *l++ & 0377;
	i += l - str;
	return i % HASHSIZE;
}

p_gram
search(type,str,option) register string str; {
	/*
	 * Search for object str.
	 * It has type UNKNOWN, LITERAL, TERMINAL or NONTERM.
	 * option can be ENTERING or BOTH (also looking).
	 */
	register int		val = 0;
	register p_entry	p;
	register int		i;
	int			type1;

	i = hash(str);
	/*
	 * Walk hash chain
	 */
	for (p = h_root[i]; p != (p_entry) 0; p = p->h_next) {
		if(!strcmp(p->h_name,str)) {
			type1 = g_gettype(&(p->h_type));
			if (type1 != type) {
				if (type1 == LITERAL || type == LITERAL) {
					continue;
				}
				if (type == TERMINAL) {
					error(linecount,
						"%s: is already a nonterminal",
						str);
					continue;
				}
				else if (type == NONTERM) {
					error(linecount,
						"%s : is already a token",
						str);
					continue;
				}
			}
			if (option==ENTERING)  {
				error(linecount,
					"%s : is already defined",str);
			}
			p->h_type.g_lineno = linecount;
			return &(p->h_type);			
		}
	}
	p = newentry(store(str), h_root[i]);
	h_root[i] = p;
	if (type == TERMINAL || type == LITERAL) {
		register p_token pt;
		
		pt = (p_token) new_mem(&token_info);
		tokens = (p_token) token_info.i_ptr;
		pt->t_string = p->h_name;
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
					val = 64*str[1] - 73*'0' +
					      8*str[2] + str[3];
				}
			} else { 
				/*
				 * No escape in literal
				 */
				if (str[1] == '\0') val = str[0];
				else error(linecount,e_literal);
			}
			pt->t_tokno = val;
			g_settype(&(p->h_type), LITERAL);
		} else {
			/*
			 * Here, type = TERMINAL
			 */
			pt->t_tokno = assval++;
			g_settype(&(p->h_type), TERMINAL);
		}
		g_setcont(&(p->h_type), ntokens);
		ntokens++;
		return &(p->h_type);
	}
	/*
	 * type == NONTERM || type == UNKNOWN 
	 * UNKNOWN and not yet declared means : NONTERM
	 */
	{
		register p_nont q;

		q = (p_nont) new_mem(&nont_info);
		nonterms = (p_nont) nont_info.i_ptr;
		q->n_name = p->h_name;
		q->n_rule = 0;
		q->n_lineno = linecount;
		q->n_string = f_input;
		q->n_follow = 0;
		q->n_flags = 0;
		q->n_contains = 0;
		g_settype(&(p->h_type), NONTERM);
		g_setcont(&(p->h_type), nnonterms);
		g_setnpar(&(p->h_type), 0);
		nnonterms++;
		return &(p->h_type);
	}
}
