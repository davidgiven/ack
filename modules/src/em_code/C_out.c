#include <em_arith.h>
#include <em_label.h>
#include <em_comp.h>
#include <em_pseu.h>
#include <em_flag.h>
#include <em_ptyp.h>
#include <em_private.h>

/* $Id$ */

static arg();
static pseudo();

extern char em_flag[];
char *C_error;

#define flags(pp)	(em_flag[(pp)->em_opcode - sp_fmnem] & EM_PAR)

struct e_instr *
C_alloc()
{
	static struct e_instr b;

	return &b;
}

int
C_out(p)
	register struct e_instr *p;
{
	/*	Generate EM-code from the e_instr structure "p"
	*/

	switch(p->em_type) {
	case EM_MNEM:
		OP(p->em_opcode);
		if (flags(p) == PAR_B && p->em_argtype == cst_ptyp) {
			p->em_ilb = p->em_cst;
			p->em_argtype = ilb_ptyp;
		}
		if (flags(p) != PAR_NO) arg(p, 0);
		NL();
		break;

	case EM_PSEU:
		pseudo(p);
		break;

	case EM_STARTMES:
		PS(ps_mes);
		CST(p->em_cst);
		break;

	case EM_MESARG:
		arg(p, 1);
		break;

	case EM_ENDMES:
		CEND();
		NL();
		break;

	case EM_DEFILB:
		DFILB(p->em_ilb);
		NL();
		break;

	case EM_DEFDLB:
		DFDLB(p->em_dlb);
		NL();
		break;

	case EM_DEFDNAM:
		DFDNAM(p->em_dnam);
		NL();
		break;
	default:
		C_error = "Illegal EM line";
		return 0;
	}
	return 1;
}

static
arg(p, comma)
	register struct e_instr *p;
{
	/*	Output the argument of "p".
	*/

	if (comma) COMMA();

	switch(p->em_argtype) {
	case 0:
		if (p->em_type == EM_MNEM && flags(p) != PAR_W) {
			abort();
		}
		CCEND();
		break;

	case ilb_ptyp:
		if (p->em_type == EM_MNEM) {
			CILB(p->em_ilb);
		}
		else {
			ILB(p->em_ilb);
		}
		break;

	case nof_ptyp:
		DOFF(p->em_dlb, p->em_off);
		break;

	case sof_ptyp:
		NOFF(p->em_dnam, p->em_off);
		break;

	case cst_ptyp:
		CST(p->em_cst);
		break;

	case pro_ptyp:
		PNAM(p->em_pnam);
		break;

	case str_ptyp:
		SCON(p->em_string, p->em_size);
		break;

	case ico_ptyp:
		WCON(sp_icon, p->em_string, p->em_size);
		break;

	case uco_ptyp:
		WCON(sp_ucon, p->em_string, p->em_size);
		break;

	case fco_ptyp:
		WCON(sp_fcon, p->em_string, p->em_size);
		break;

	default:
		abort();
	}
}

static
pseudo(p)
	register struct e_instr *p;
{

	PS(p->em_opcode);

	switch(p->em_opcode) {
	case ps_exc:
		CST(p->em_exc1);
		COMMA();
		CST(p->em_exc2);
		break;

	case ps_rom:
	case ps_con:
		arg(p, 0);
		CEND();
		break;

	case ps_bss:
		CST(EM_bsssize);
		arg(p, 1);
		COMMA();
		CST((arith) EM_bssinit);
		break;

	case ps_hol:
		CST(EM_holsize);
		arg(p, 1);
		COMMA();
		CST((arith) EM_holinit);
		break;

	case ps_pro:
		arg(p, 0);
		COMMA();
		if (p->em_nlocals != -1) CST(p->em_nlocals);
		else	CCEND();
		break;

	case ps_end:
		if (p->em_argtype == 0) CCEND();
		else CST(p->em_cst);
		break;

	default:
		arg(p, 0);
		break;
	}

	NL();
}
