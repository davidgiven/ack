/* $Header$ */
#include <stdio.h>
#include <em_spec.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include <em_flag.h>
#include <em_ptyp.h>
#include <em_mes.h>
#include <alloc.h>
#include <em.h>
#include <em_comp.h>
#include <system.h>
#include <emO_code.h>

#define OTHER	0
#define op_lab	sp_fpseu

typedef struct e_instr *p_instr;

#define FLUSHDFA()	if(OO_state) {\
				*OO_nxtpatt++ = OO_OTHER; OO_dfa(OTHER);\
			} else if(OO_noutput) OO_flush();

#define GETINSTR() ((p_instr)st_alloc((char **)&OO_freeq,sizeof(struct e_instr),20))
#define OO_free(p) st_free((p),&OO_freeq,sizeof(struct e_instr))

extern p_instr	OO_freeq;
extern p_instr	*OO_patternqueue;
extern p_instr	*OO_nxtpatt;
extern p_instr	*OO_bkupqueue;
extern p_instr	*OO_nxtbackup;
extern p_instr	OO_OTHER;
extern int	OO_state;
extern int	OO_noutput;	/* number of instructions in output queue */
extern arith	OO_WSIZE;	/* wordlength */
extern arith	OO_PSIZE;	/* pointer length */
#ifdef STATS
extern int	OO_wrstats;			/* statistics output */
#endif

extern char	*OO_freestr();
extern arith	OO_rotate();
extern arith	OO_offset();

#define CST(p)		(p->em_cst)
#define PNAM(p)		(p->em_pnam)
#define LAB(p)		(p->em_ilb)
#define DEFILB(p)	(p->em_ilb)

