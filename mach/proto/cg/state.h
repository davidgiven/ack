/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef STATE_H_
#define STATE_H_


#define STONSTACK	/* if defined state is saved in stackframe */

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

#ifndef STONSTACK
state_p state_p savestatus(void);
#endif

void restorestatus(register state_p sp);
state_p savestatus(register state_p sp);

#endif /* STATE_H_ */
