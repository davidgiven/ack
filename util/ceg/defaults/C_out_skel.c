/* $Id$ */
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
char *C_error;

PRIVATE C_dopseudo();

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
			if ((em_flag[p->em_opcode-sp_fmnem] & EM_PAR) == PAR_W &&
			    p->em_argtype == 0) {
#include "C_mnem_narg"
			} else {
#include "C_mnem"
			}

			break;
		case EM_DEFILB:
			/* defining occurrence of an instruction label */
			C_df_ilb(p->em_ilb);
			break;
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
			C_dopseudo(p);
			break;
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

PRIVATE
C_dopseudo(p)
	register struct e_instr *p;
{

	switch(p->em_opcode) {
		case ps_exc: {
			C_error = "EXC not allowed";
			break;
		}
		case ps_hol: {
			switch(p->em_argtype) {
			    case cst_ptyp:
				C_hol_cst(EM_holsize,
					  p->em_cst,
					  EM_holinit);
				break;
			    case ico_ptyp:
				C_hol_icon(EM_holsize,
					   p->em_string,
					   p->em_size, 
					   EM_holinit);
				break;
			    case uco_ptyp:
				C_hol_ucon(EM_holsize,
					   p->em_string,
					   p->em_size,
					   EM_holinit);
				break;
			    case fco_ptyp:
				C_hol_fcon(EM_holsize,
					   p->em_string,
					   p->em_size, 
					   EM_holinit);
				break;
			    case sof_ptyp:
				C_hol_dnam(EM_holsize,
					   p->em_dnam,
					   p->em_off, 
					   EM_holinit);
				break;
			    case nof_ptyp:
				C_hol_dlb(EM_holsize,
					  p->em_dlb,
					  p->em_off, 
					  EM_holinit);
				break;
			    case ilb_ptyp:
				C_hol_ilb(EM_holsize,
					  p->em_ilb, 
					  EM_holinit);
				break;
			    case pro_ptyp:
				C_hol_pnam(EM_holsize,
					   p->em_pnam, 
					   EM_holinit);
				break;
			    default:
				C_error = "Illegal parameter type";
				break;
			}
			break;
		}
		case ps_bss: {
			switch(p->em_argtype) {
			    case cst_ptyp:
				C_bss_cst(EM_bsssize,
					  p->em_cst,
					  EM_bssinit);
				break;
			    case ico_ptyp:
				C_bss_icon(EM_bsssize,
					   p->em_string,
					   p->em_size, 
					   EM_bssinit);
				break;
			    case uco_ptyp:
				C_bss_ucon(EM_bsssize,
					   p->em_string,
					   p->em_size,
					   EM_bssinit);
				break;
			    case fco_ptyp:
				C_bss_fcon(EM_bsssize,
					   p->em_string,
					   p->em_size, 
					   EM_bssinit);
				break;
			    case sof_ptyp:
				C_bss_dnam(EM_bsssize,
					   p->em_dnam,
					   p->em_off, 
					   EM_bssinit);
				break;
			    case nof_ptyp:
				C_bss_dlb(EM_bsssize,
					  p->em_dlb,
					  p->em_off, 
					  EM_bssinit);
				break;
			    case ilb_ptyp:
				C_bss_ilb(EM_bsssize,
					  p->em_ilb, 
					  EM_bssinit);
				break;
			    case pro_ptyp:
				C_bss_pnam(EM_bsssize, 
					   p->em_pnam, 
					   EM_bssinit);
				break;
			    default:
				C_error = "Illegal parameter type";
				break;
			}
			break;
		}
		case ps_end:
			if (p->em_argtype != 0) {
				C_end(p->em_cst);
				break;
			}
			C_end_narg();
			break;
		case ps_exa:
		case ps_ina:
			if (p->em_argtype == nof_ptyp) {
				if (p->em_opcode == ps_exa) {
					C_exa_dlb(p->em_dlb);
				}
				else	C_ina_dlb(p->em_dlb);
				break;
			}
			if (p->em_opcode == ps_exa) {
				C_exa_dnam(p->em_dnam);
			}
			else	C_ina_dnam(p->em_dnam);
			break;
		case ps_exp:
			C_exp(p->em_pnam);
			break;
		case ps_inp:
			C_inp(p->em_pnam);
			break;
		case ps_pro:
			if (p->em_nlocals >= 0) {
				C_pro(p->em_pnam, p->em_nlocals);
			}
			else	C_pro_narg(p->em_pnam);
			break;
		case ps_con:
			switch(p->em_argtype) {
				case ilb_ptyp:
					C_con_ilb(p->em_ilb);
					break;
				case nof_ptyp:
					C_con_dlb(p->em_dlb, p->em_off);
					break;
				case sof_ptyp:
					C_con_dnam(p->em_dnam, p->em_off);
					break;
				case cst_ptyp:
					C_con_cst(p->em_cst);
					break;
				case pro_ptyp:
					C_con_pnam(p->em_pnam);
					break;
				case str_ptyp:
					C_con_scon(p->em_string, p->em_size);
					break;
				case ico_ptyp:
					C_con_icon(p->em_string, p->em_size);
					break;
				case uco_ptyp:
					C_con_ucon(p->em_string, p->em_size);
					break;
				case fco_ptyp:
					C_con_fcon(p->em_string, p->em_size);
					break;
				default:
					C_error = "Illegal argument type";
					return;
			}
			break;
		case ps_rom:
			switch(p->em_argtype) {
				case ilb_ptyp:
					C_rom_ilb(p->em_ilb);
					break;
				case nof_ptyp:
					C_rom_dlb(p->em_dlb, p->em_off);
					break;
				case sof_ptyp:
					C_rom_dnam(p->em_dnam, p->em_off);
					break;
				case cst_ptyp:
					C_rom_cst(p->em_cst);
					break;
				case pro_ptyp:
					C_rom_pnam(p->em_pnam);
					break;
				case str_ptyp:
					C_rom_scon(p->em_string, p->em_size);
					break;
				case ico_ptyp:
					C_rom_icon(p->em_string, p->em_size);
					break;
				case uco_ptyp:
					C_rom_ucon(p->em_string, p->em_size);
					break;
				case fco_ptyp:
					C_rom_fcon(p->em_string, p->em_size);
					break;
				default:
					C_error = "Illegal argument type";
					return;
			}
			break;
		default: 
			C_error = "Illegal pseudo instruction";
			break;
	}
}
