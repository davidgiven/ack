/* $Header$ */
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
#include <idf_pkg.spec>
#include <emO_code.h>

#define NULL 0
#define FLUSHDFA()	if(OO_state) { OO_inop(OTHER); OO_dfa(OTHER); } \
			else if(OO_noutput) OO_flush();
#define NEXTEM()	((OO_nxtbackup>OO_bkupqueue)?\
				((*OO_nxtpatt++ = *(--OO_nxtbackup))->opcode):\
				0)

#define op_lab 255
#define OTHER  254
#define none_ptyp 0


struct e_instr *EM_getinstr();

struct instr {
	int opcode;
	int argtype;
	union {
		arith cst;
		label lab;
		char  *pnam;
		struct {
			label dlb;
			arith noff;
		} ndlb;
		struct {
			char  *dnam;
			arith soff;
		} sdlb;
	} val;
#define acst val.cst
#define alab val.lab
#define apnam val.pnam
#define adlb val.ndlb.dlb
#define anoff val.ndlb.noff
#define adnam val.sdlb.dnam
#define asoff val.sdlb.soff
};

extern struct instr **OO_patternqueue;
extern struct instr **OO_nxtpatt;
extern struct instr **OO_bkupqueue;
extern struct instr **OO_nxtbackup;
extern int OO_state;
extern int OO_noutput;	/* number of instructions in output queue */
extern int OO_WSIZE;	/* wordlength */
extern int OO_PSIZE;	/* pointer length */
#ifdef STATS
extern int OO_wrstats;			/* statistics output */
#endif

#define CST(p)		(p->acst)
#define PNAM(p)		(p->apnam)
#define LAB(p)		(p->alab)
#define DEFILB(p)	(p->alab)
