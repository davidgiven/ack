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

# define GETNXTPATT() (OO_nxtpatt>&OO_buffer[MAXBUFFER]?OO_halfflush():OO_nxtpatt++)
# define GETNXTREPL() (OO_nxtrepl++)

# define FLUSHDFA()	(GETNXTPATT())->em_opcode=OTHER;\
			if (OO_state) OO_dfa(OTHER); else OO_flush()

extern int	OO_state;
extern arith	OO_WSIZE;	/* wordlength */
extern arith	OO_PSIZE;	/* pointer length */
#ifdef STATS
extern int	OO_wrstats;			/* statistics output */
#endif

extern char	*OO_freestr();
extern arith	OO_rotate();
extern arith	OO_offset();

#define CST(p)		(p.em_cst)
#define PNAM(p)		(p.em_pnam)
#define LAB(p)		(p.em_ilb)
#define DEFILB(p)	(p.em_ilb)
#define DEFINED(p)	(p.em_argtype)

