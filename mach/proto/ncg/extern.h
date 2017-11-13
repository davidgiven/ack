/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

extern int maxply;                      /* amount of lookahead allowed */
extern int stackheight;                 /* # of tokens on fakestack */
extern token_t fakestack[];             /* fakestack itself */
extern int nallreg;                     /* number of allocated registers */
extern int allreg[];                    /* array of allocated registers */
extern token_p curtoken;                /* pointer to current token */
extern result_t dollar[];               /* Values of $1,$2 etc.. */
extern int nemlines;                    /* # of EM instructions in core */
extern struct emline emlines[];         /* EM instructions itself */
extern struct emline *emp;              /* pointer to current instr */
extern struct emline *saveemp;		/* pointer to start of pattern */
extern int tokpatlen;                   /* length of current stackpattern */
extern rl_p curreglist;                 /* side effect of findcoerc() */
#ifndef NDEBUG
extern int Debug;                       /* on/off debug printout */
#endif

/*
 * Next descriptions are external declarations for tables created
 * by bootgram.
 * All definitions are to be found in tables.c (Not for humans)
 */

extern byte coderules[];                /* pseudo code for cg itself */
extern char stregclass[];               /* static register class */
extern struct reginfo machregs[];       /* register info */
extern tkdef_t tokens[];                /* token info */
extern node_t enodes[];                 /* expression nodes */
extern string codestrings[];            /* table of strings */
extern set_t machsets[];                /* token expression table */
extern inst_t tokeninstances[];         /* token instance description table */
extern move_t moves[];                  /* move descriptors */
extern test_t tests[];                  /* test descriptors */
extern byte pattern[];                  /* EM patterns */
extern int pathash[256];                /* Indices into previous */
extern c1_t c1coercs[];                 /* coercions type 1 */
#ifdef MAXSPLIT
extern c2_t c2coercs[];                 /* coercions type 2 */
#endif /* MAXSPLIT */
extern c3_t c3coercs[];                 /* coercions type 3 */
extern struct reginfo **reglist[];	/* lists of registers per property */

#define eqregclass(r1,r2) (stregclass[r1]==stregclass[r2])

#ifdef REGVARS
extern int nregvar[];			/* # of register variables per type */
extern int *rvnumbers[];		/* lists of numbers */
#endif

extern FILE *codefile;
