/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	makecalls: expand a datastructure as delivered by "EM_getline"
	into calls to the procedural interface.
	Exported routine:
		C_out
*/

#define CODE_EXPANDER
#define EXPORT
#define PRIVATE static

#include <em_spec.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include <em_flag.h>
#include "em_ptyp.h"
#include <em.h>
#include <em_comp.h>
#include <assert.h>

extern char em_flag[];	/* One per EM instruction: indicates parameter kind */
extern short em_ptyp[];	/* One per parameter kind: indicates parameter type */
static char *C_error;

EXPORT int
C_out(p)
	register struct e_instr *p;
{
	C_error = 0;
	switch(p->em_type) {
		default:
			C_error = "Illegal EM line";
			break;
		case EM_MNEM:
			/* normal instruction */
{
	register int parametertype;	/* parametertype of the instruction */

	parametertype = em_flag[p->em_opcode-sp_fmnem] & EM_PAR;
	switch(parametertype) {
		default:
			break;
		case PAR_W:
			if (p->em_argtype != 0) {
			}
			else {
#include "C_mnem_narg"
			}
			break;
	}
#include "C_mnem"
}

			break;
		case EM_DEFILB:
			/* defining occurrence of an instruction label */
			C_df_ilb(p->em_ilb);
			break;
#ifdef ____
		case EM_DEFDLB:
			/* defining occurrence of a global data label */
			C_df_dlb(p->em_dlb);
			break;
		case EM_DEFDNAM:
			/* defining occurrence of a non-numeric data label */
			C_df_dnam(p->em_dnam);
			break;
		case EM_PSEU:
			/* pseudo */
			EM_dopseudo(p);
			break;
#endif
		case EM_STARTMES:
			/* start of a MES pseudo */
			C_mes_begin((int) (p->em_cst));
			break;
		case EM_MESARG:
			switch(p->em_argtype) {
			case ilb_ptyp:
				C_ilb(p->em_ilb);
				break;
			case nof_ptyp:
				C_dlb(p->em_dlb, p->em_off);
				break;
			case sof_ptyp:
				C_dnam(p->em_dnam, p->em_off);
				break;
			case cst_ptyp:
				C_cst(p->em_cst);
				break;
			case pro_ptyp:
				C_pnam(p->em_pnam);
				break;
			case str_ptyp:
				C_scon(p->em_string, p->em_size);
				break;
			case ico_ptyp:
				C_icon(p->em_string, p->em_size);
				break;
			case uco_ptyp:
				C_ucon(p->em_string, p->em_size);
				break;
			case fco_ptyp:
				C_fcon(p->em_string, p->em_size);
				break;
			default:
				C_error = "Illegal argument type";
				break;
			}
			break;
		case EM_ENDMES:
			C_mes_end();
			break;
	}
	if (C_error) return 0;
	return 1;
}
