/* $Id$ */
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

#define MAXBUFFER	200
#define MAXSTRING	1000

#define OTHER	0
#define op_lab	sp_fpseu

typedef struct e_instr *p_instr;

struct dfa {
	short	check;
	short	next;
};

struct dodefault {
	short	numout;
	short	next;
};

extern p_instr	OO_buffer;
extern p_instr	OO_patternqueue;
extern p_instr	OO_nxtpatt;
extern p_instr	OO_endbackup;
extern p_instr	OO_nxtrepl;

p_instr OO_halfflush();

# define GETNXTPATT() (OO_nxtpatt>=&OO_buffer[MAXBUFFER]?OO_halfflush():OO_nxtpatt++)
# define GETNXTREPL() (OO_nxtrepl++)

# define FLUSHDFA()	(GETNXTPATT())->em_opcode=OTHER;\
			if (OO_state) OO_dfa(OTHER); else OO_flush()

extern int	OO_state;
extern arith	OO_WSIZE;	/* wordlength */
extern arith	OO_DWSIZE;	/* 2*wordlength */
extern arith	OO_PSIZE;	/* pointer length */
#ifdef STATS
extern int	OO_wrstats;			/* statistics output */
#endif

#define CST(p)		(p.em_cst)
#define PNAM(p)		(p.em_pnam)
#define LAB(p)		(p.em_ilb)
#define DEFILB(p)	(p.em_ilb)
#define DEFINED(p)	(p.em_argtype)

void EM_mkop(p_instr, int);
void EM_mknarg(p_instr, int);
void EM_mkilb(p_instr, int, label);
void EM_mknof(p_instr, int, label, arith);
void EM_mksof(p_instr, int, char *, arith);
void EM_mkcst(p_instr, int, arith);
void EM_mkpro(p_instr, int, char *);
void EM_mkdefilb(p_instr, int, label);
void EM_Nop(int);
void EM_Nnarg(int);
void EM_Nilb(int, label);
void EM_Nnof(int, label, arith);
void EM_Nsof(int, char *, arith);
void EM_Ncst(int, arith);
void EM_Npro(int, char *);
void EM_Ndefilb(int, label);
void EM_Rop(int);
void EM_Rnarg(int);
void EM_Rilb(int, label);
void EM_Rnof(int, label, arith);
void EM_Rsof(int, char *, arith);
void EM_Rcst(int, arith);
void EM_Rpro(int, char *);
void EM_Rdefilb(int, label);

arith OO_rotate(arith, arith);
int OO_signsame(arith, arith);
int OO_sfit(arith, arith);
int OO_ufit(arith, arith);
int OO_extsame(p_instr, p_instr);
int OO_namsame(p_instr, p_instr);
arith OO_offset(p_instr);

char	*OO_freestr(char *);
void OO_dfa(int);
void OO_flush(void);
p_instr OO_halfflush(void);
void OO_mkext(p_instr, int, p_instr, arith);
void OO_mkrepl(int, int, int);

#ifdef DEBUG
void dumpstate(char *);
void prtinst(p_instr);
#endif
