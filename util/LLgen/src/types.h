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
 * $Header$
 * Type and structure definitions
 */

typedef	int	*p_set;		/* pointer to bitset */
typedef char	*p_mem;		/* pointer to some core */
typedef char	*string;

/*
 * structure for a token
 */
typedef struct token {
	int	t_tokno;	/* Lexical token number */
	union {
		string	t_s;	/* Attribute is either a string or */
		int	t_v;	/* an integer */
	} t_x;
# define t_string t_x.t_s
# define t_num t_x.t_v
	int	t_flags;
	int	t_next;
	int	t_lineno;
} t_token, *p_token;

/*
 * structure for the grammar elements
 */
typedef struct gram {
	short	x;		/* for lay-out see comment below */
	short	g_lineno;	/* element found on this line number */
	union {
		int	g_index;
		struct term *	g_term;
		struct link *	g_link;
	} g_i;
} t_gram,*p_gram;

/*
 * Layout of the field x:
 *
 * 15  ....... 7 6 ........ 3 2 .... 0
 * -----------------------------------
 * | unused    | | nparams  | | type |
 * -----------------------------------
 */
# define EORULE		00	/* End of (sub)rule */
# define ACTION		01	/* Imbedded action */
# define NONTERM	02	/* A nonterminal */
# define TERMINAL	03	/* A terminal */
# define TERM		04	/* Something between square brackets */
# define ALTERNATION	05	/* Alternation (|) */
# define LITERAL	06	/* Also a terminal */

/*
 * Access macros for the x-field of a grammar element
 */
# define g_gettype(p)	((p)->x&07)
# define g_getcont(p)	((p)->g_i.g_index)
# define g_getterm(p)	((p)->g_i.g_term)
# define g_getlink(p)	((p)->g_i.g_link)
# define g_getnpar(p)	(((p)->x>>3)&017)
# define g_settype(p,s)	{ assert(((unsigned)(s))<=07);(p)->x=((p)->x&~07)|(s);}
# define g_setcont(p,s) ((p)->g_i.g_index=(s))
# define g_setterm(p,s)	((p)->g_i.g_term = (s))
# define g_setlink(p,s)	((p)->g_i.g_link = (s))
# define g_setnpar(p,s) { assert(((unsigned)(s))<=017);(p)->x=((p)->x&~0170)|((s)<<3);}

/*
 * Some constants to communicate with the symbol table search routine
 */
# define UNKNOWN	00	/* Not equal to NONTERM, TERMINAL or LITERAL */

/*
 * Some constants for safety
 */
# define SAFE		0	/* Indicates that a scan is done, and that the
				 * token is correct
				 */
# define SAFESCANDONE	1	/* Indicates that a scan is done, and that the
				 * token will not be skipped
				 */
# define SCANDONE	2	/* Indicates that a scan is done */
# define NOSCANDONE	3	/* Indicates that no scan is done */
# define NOSAFETY	4	/* Safety not yet computed */

/*
 * nonterminal structure
 */
typedef	struct {
	short	n_flags;	/* low order four bits are reserved
				 * the parameter count
				 */
# define getntparams(p)	((p)->n_flags&017)
# define setntparams(p,i)	{assert(((unsigned)(i))<=017);(p)->n_flags&=~017;(p)->n_flags|=(i);}
# define GENSTATIC	01000	/* set if routine can be made static */
# define RECURSIVE	02000	/* Set if the default rule is recursive */
# define PARAMS		04000	/* tells if a nonterminal has parameters */
# define EMPTY		010000	/* tells if a nonterminal produces empty */
# define LOCALS		020000  /* local declarations ? */
# define REACHABLE	040000	/* can this nonterminal be reached ? */
# define VERBOSE	0100000	/* Set if in LL.output file */
	char	n_insafety;
	char	n_outsafety;
# define getntsafe(p)	((p)->n_insafety)
# define setntsafe(p,i)	{assert(((unsigned)(i))<=NOSAFETY);(p)->n_insafety=(i);}
# define getntout(p)	((p)->n_outsafety)
# define setntout(p,i)	{assert(((unsigned)(i))<=NOSAFETY);(p)->n_outsafety=(i);}
	short	n_count;	/* pieces of code before this rule */
	short	n_lineno;	/* declared on line ... */
	p_gram	n_rule;		/* pointer to right hand side of rule */
	union {
		p_set	n_f;	/* ptr to "first" set */
		string	n_s;	/* If this nonterminal is not declared,
				 * This field indicates the filename in
				 * which it occurred
				 */
	} n_x;
# define n_first  n_x.n_f
# define n_string n_x.n_s
	p_set	n_follow;	/* pointer to the "follow" set	*/
	p_set	n_contains;	/* pointer to symbols that can be produced */
	string	n_name;		/* name of nonterminal */
	int	n_next;		/* index of next nonterminal */
	long	n_off;		/* index of parameters in action file */
} t_nont, *p_nont;

/* 
 * hash table structure
 */
typedef struct h_entry {
	string		h_name;		/* pointer to name */
	t_gram		h_type;		/* Type and index */
	struct h_entry	*h_next;	/* next in hash chain */
} t_entry, *p_entry;

/*
 * link	structure to link alternations
 */
typedef struct link {
	unsigned int	l_flag;
# define COND		01		/* Set if this alternative has a %if */
# define DEF		02		/* This alternative is default */
# define PREFERING	010		/* %prefer */
# define AVOIDING	020		/* %avoid */
# define NOCONF		01000		/* Set if there is a resolver without
					 * conflict
					 */
	p_gram		l_rule;		/* pointer to this rule	*/
	p_set		l_symbs;	/* set,	when to take this rule */
	p_set		l_others;	/* set, when to take another rule */
} t_link, *p_link;

/*
 * Structure for a repitition specification
 */
typedef short t_reps,*p_reps;

# define FIXED		00	/* a fixed number */
# define STAR		01	/* 0 or more times */
# define PLUS		02	/* 1 or more times */
# define OPT		03	/* 0 or 1 times */

/*
 * Access macros for repitition in term
 */
# define r_getkind(q)	((q)->t_repeats&03)
# define r_getnum(q)	(((q)->t_repeats>>2)&037777)
# define r_setkind(q,s)	{ assert(((unsigned)(s))<=03);(q)->t_repeats=((q)->t_repeats&0177774)|(s);}
# define r_setnum(q,s)	{ assert(((unsigned)(s))<=037777);(q)->t_repeats=((q)->t_repeats&03)|((s)<<2);}

/*
 * header structure for	a term
 */
typedef struct term {
	t_reps	t_repeats;
	short	t_flags;	/* Low order three bits for safety */
# define gettout(q)	((q)->t_flags&07)
# define settout(q,i)	{assert(((unsigned)(i))<=NOSAFETY);(q)->t_flags&=~07;(q)->t_flags|=i;}
# define PERSISTENT	010	/* Set if this term has %persistent */
# define RESOLVER	020	/* Set if this term has %while */
# define EMPTYFIRST	0100	/* Error, empty first */
# define EMPTYTERM	0200	/* Error, term can produce empty */
/* # define NOCONF	01000	see link structure */

	p_gram	t_rule;		/* pointer to this term	*/
	p_set	t_follow;	/* set of followers */
	p_set	t_first;	/* set of firsts */
	p_set	t_contains;	/* contains set */
} t_term, *p_term;

/*
 * structure for firstmacros list
 */
typedef struct ff_firsts {
	string	ff_name;	/* Name of the macro */
	int	ff_nont;	/* for this nonterminal */
	struct ff_firsts *ff_next;
} t_first, *p_first;

/*
 * structure for start symbol list
 */
typedef t_first t_start;
typedef p_first p_start;

/*
 * structure for file names and info
 */
typedef struct f_file {
	string	f_name;		/* File name */
	p_first f_firsts;	/* ptr to list of firstmacros that must be
				 * generated in the target file for this
				 * grammar file
				 */
	int f_nonterminals;	/* list of nonterminals in this file  */
	int f_terminals;	/* list of terminals in this file  */
	p_set f_used;		/* set of nonterminals used in this file */
} t_file, *p_file;

typedef struct info_alloc {
	/*
	 * Structure used for dynamically growing arrays
	 */
	unsigned i_size;	/* Size of the array */
	unsigned i_esize;	/* Size of an element */
	unsigned i_incr;	/* When filled, add room for i_incr elements */
	p_mem	i_ptr;		/* ptr to base of array */
	p_mem	i_max;		/* ptr to first free */
	p_mem	i_top;		/* ptr to top of array */
} t_info, *p_info;

# ifdef NDEBUG
# define STATIC static
# else /* not NDEBUG */
# define STATIC extern
# endif /* not NDEBUG */
