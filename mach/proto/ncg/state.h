/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

typedef struct state {
	struct state *st_next;                  /* for linked list */
	int st_sh;                              /* stackheight */
	token_t st_fs[MAXFSTACK];               /* fakestack */
	int st_na;                              /* nallreg */
	int st_ar[MAXALLREG];                   /* allreg[] */
	token_p st_ct;                          /* curtoken */
	result_t st_do[LONGESTPATTERN];         /* dollar[] */
	struct reginfo st_mr[NREGS];            /* machregs[] */
	int st_ne;                              /* nemlines */
	struct emline st_el[MAXEMLINES];        /* emlines[] */
	struct emline *st_em;                   /* emp */
	struct emline *st_se;			/* saveemp */
	int st_tl;				/* tokpatlen */
	int st_ns;				/* nstab */
} state_t,*state_p;

/* state.c */
void savestatus(state_p);
void restorestatus(state_p);
