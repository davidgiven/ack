/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

typedef struct cost {
	short ct_space;
	short ct_time;
} cost_t,*cost_p;

typedef struct {
	string ea_str;
	word ea_off;
} addr_t;

typedef struct {
	int     t_token;        /* kind of token, -1 for register */
	union {
		word aw;	/* integer type */
		addr_t aa;	/* address type */
		int ar;		/* register type */
	} t_att[TOKENSIZE];
} token_t,*token_p;

struct reginfo {
	int     r_repr;                 /* index in string table */
	int     r_size;                 /* size in bytes */
#if MAXMEMBERS!=0
	int     r_members[MAXMEMBERS];  /* register contained within this reg */
	short	r_clash[REGSETSIZE];	/* set of clashing registers */
	int	r_iclash;		/* index in clashlist; other represen-
					   tation of r_clash
					*/
#endif
	int     r_refcount;             /* Times in use */
	token_t r_contents;             /* Current contents */
	int     r_tcount;               /* Temporary count difference */
};

#if MAXMEMBERS!=0
#define clash(a,b) ((machregs[a].r_clash[(b)>>4]&(1<<((b)&017)))!=0)
#else
#define clash(a,b) ((a)==(b))
#endif

typedef struct {
	int     t_size;                 /* size in bytes */
	cost_t  t_cost;                 /* cost in bytes and time */ 
	byte    t_type[TOKENSIZE];      /* types of attributes, TT_??? */
	int     t_format;               /* index of formatstring */
} tkdef_t,*tkdef_p;

struct emline {
	int     em_instr;
	int     em_optyp;
	string  em_soper;
	union {
		word    em_ioper;
		long	em_loper;
	} em_u;
};

#define OPNO 0
#define OPINT 1
#define OPSYMBOL 2

typedef struct {
	int rl_n;       /* number in list */
	int rl_list[NREGS];
} rl_t,*rl_p;
