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

#define NULL 0
#define FLUSHDFA()	if(state) { inop(OTHER); dfa(OTHER); } \
			else if(noutput) flush();

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

extern struct instr **patternqueue, **nextpatt;
extern int state;
extern int noutput;	/* number of instructions in output queue */
extern int WSIZE;	/* wordlength */
extern int PSIZE;	/* pointer length */
#ifdef DEBUG
extern int dflag;			/* debugging output */
#endif
#ifdef STATS
extern int sflag;			/* statistics output */
#endif

#define CST(p)		(p->acst)
#define PNAM(p)		(p->apnam)
#define LAB(p)		(p->alab)
#define DEFILB(p)	(p->alab)
