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
 * types.h $Header$
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
} t_token;

/*
 * structure for the grammar elements
 */
typedef struct gram {
	short	x;		/* for lay-out see comment below */
	short	g_lineno;	/* element found on this line number */
} t_gram,*p_gram;

/*
 * Layout of the field x:
 *
 * 15     13 12         9 8                 0
 * ------------------------------------------
 * | type   | nparams    | index            |
 * ------------------------------------------
 * of
 * 15     13 12                             0
 * ------------------------------------------
 * | type   | index                         |
 * ------------------------------------------
 * dependant on type
 */
# define UNDEFINED	000777
# define EORULE		00	/* End of (sub)rule */
# define ACTION		01	/* Imbedded action */
# define NONTERM	02	/* A nonterminal */
# define TERMINAL	03	/* A terminal */
# define TERM		04	/* Something between square brackets */
# define ALTERNATION	05	/* Alternation (|) */

/*
 * Access macros for the x-field of a grammar element
 */
# define g_init(p)	{(p)->x = 0;}
# define g_gettype(p)	(((p)->x>>13)&07)
# define g_getcont(p)	((p)->x&017777)
# define g_getnont(p)	((p)->x&0777)
# define g_getnpar(p)	(((p)->x>>9)&07)
# define g_settype(p,s)	{ assert(((unsigned)(s))<=07);(p)->x=((p)->x&017777)|((s)<<13);}
# define g_setcont(p,s) { assert(((unsigned)(s))<=017777);(p)->x=((p)->x&0160000)|(s);}
# define g_setnont(p,s) { assert(((unsigned)(s))<=0777);(p)->x=((p)->x&0177000)|(s);}
# define g_setnpar(p,s) { assert(((unsigned)(s))<=07);(p)->x=((p)->x&0170777)|((s)<<9);}

/*
 * Some constants to communicate with the symbol table search routine
 */
# define LITERAL	01	/* Not equal to NONTERM or TERMINAL */
# define UNKNOWN	00	/* Not equal to NONTERM, TERMINAL or LITERAL */

/*
 * Some constants for safety
 */
# define SAFE		0
# define SAFESCANDONE	1
# define SCANDONE	2
# define NOSCANDONE	3
# define NOSAFETY	4

/*
 * nonterminal structure
 */
typedef	struct {
	short	n_flags;	/* low order three bits are reserved
				 * for "safety" information,
				 * the next three bits are reserved for
				 * the parameter count
				 */
# define getntsafe(f)	((f)&07)
# define setntsafe(p,i)	{assert(((unsigned)(i))<=7);*(p)&=~07;*(p)|=(i);}

# define RECURSIVE	00100	/* Set if the default rule is recursive */
# define NNOSCAN	00200	/* Set if the nonterminal does not scan ahead */
# define CONTIN		00400	/* continuation already computed? */
# define BUSY		01000	/* or are we busy computing it? */
# define PARAMS		02000	/* tells if a nonterminal has parameters */
# define PRODUCE	04000	/* tells if a nonterminal produces anything */
# define EMPTY		010000	/* tells if a nonterminal produces empty */
# define LOCALS		020000  /* local declarations ? */
# define REACHABLE	040000	/* can this nonterminal be reached ? */
# define VERBOSE	0100000	/* Set if in LL.output file */
	short	n_count;	/* pieces of code before this rule */
	int	n_lineno;	/* declared on line ... */
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
} t_nont, *p_nont;

/* 
 * hash table structure
 */
typedef struct h_entry {
	string		h_name;		/* pointer to name */
	int		h_num;		/* numbering of terminals */
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
 * Access macros for repitition
 */
# define r_init(p)	{*(p)=0;}
# define r_getkind(p)	(*(p)&03)
# define r_getnum(p)	((*(p)>>2)&037777)
# define r_setkind(p,s)	{ assert(((unsigned)(s))<=03);*(p)=(*(p)&0177774)|(s);}
# define r_setnum(p,s)	{ assert(((unsigned)(s))<=037777);*(p)=(*(p)&03)|((s)<<2);}

/*
 * header structure for	a term
 */
typedef struct term {
	t_reps	t_reps;		/* repeats ? */
	short	t_flags;
# define PERSISTENT	010	/* Set if this term has %persistent */
# define RESOLVER	020	/* Set if this term has %while */
# define EMPTYFIRST	0100	/* Error, empty first */
# define EMPTYTERM	0200	/* Error, term can produce empty */
# define TNOSCAN	0400	/* Set if this term does not scan ahead */
/* # define NOCONF	01000	see link structure */

	p_gram	t_rule;		/* pointer to this term	*/
	p_set	t_follow;	/* set of followers */
	p_set	t_first;	/* set of firsts */
	p_set	t_contains;
} t_term, *p_term;

/*
 * structure for firstmacros list
 */
typedef struct ff_firsts {
	string	ff_name;	/* Name of the macro */
	p_nont	ff_nont;	/* for this nonterminal */
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
	short	*f_start,*f_end;/* pointers in the "order" array,
				 * Indicating which nonterminals were defined
				 * in this file
				 */
} t_file, *p_file;
# ifdef NDEBUG
# define STATIC static
# else not NDEBUG
# define STATIC extern
# endif not NDEBUG
