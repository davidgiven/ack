/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	makecalls: expand a datastructure as delivered by "EM_getline"
	into calls to the procedural interface.
	Exported routine:
		EM_mkcalls
*/

#include <em_spec.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include <em_flag.h>
#include "em_ptyp.h"
#include <em.h>
#include "em_comp.h"
#include <assert.h>

extern char em_flag[];	/* One per EM instruction: indicates parameter kind */
extern short em_ptyp[];	/* One per parameter kind: indicates parameter type */

static int listtype = 0;	/* indicates pseudo when generating code for
				   variable length argument lists
				   (only for MES)
				*/

#ifdef CHECKING
/*	c_getarg: Check the argument indicated by "args".
	The argument must be of a type allowed by "typset".
	Return a pointer to the next argument.
*/
PRIVATE struct e_args *
c_getarg(args, typset)
	register struct e_args *args;
{

	if (((!typset) && args) ||
	    ((!args) && typset)) {
		/* End of arguments expected, but there are more, or
		   an argument expected, but there is none
		*/
		EM_error = "Illegal number of parameters";
		return 0;
	}

	if (!args) return 0;
	if (!(args->em_argtype & typset)) {
		/* Type error */
		EM_error = "Illegal parameter type";
	}

	return args->em_next;
}
#else not CHECKING
#define c_getarg(arg, x)	((arg) ? (arg)->em_next : (arg))
#endif CHECKING

/*	EM_doinstr: An EM instruction
*/
PRIVATE
EM_doinstr(opcode, arg)
	register struct e_args *arg;
{
	register int parametertype;	/* parametertype of the instruction */
	register struct e_args *args;

	parametertype = em_flag[opcode-sp_fmnem] & EM_PAR;
#ifdef CHECKING
	if (parametertype != PAR_NO && parametertype != PAR_W) {
		if (!arg) {
			EM_error = "Illegal number of parameters";
			return;
		}
	}
#endif CHECKING
	switch(parametertype) {
		case PAR_NO:
			break;
		default:
			args = c_getarg(arg, em_ptyp[parametertype]);
			args = c_getarg(args, 0);
			break;
		case PAR_W:
			if (arg) {
				args = c_getarg(arg, cst_ptyp);
				args = c_getarg(args, 0);
			}
			else {
#include "C_mnem_narg"
				return;
			}
			break;
	}
#include "C_mnem"
}

PRIVATE
EM_dopseudo(opcode, args)
	register struct e_args *args;
{
	register struct e_args *arg;

	switch(opcode) {
		case ps_exc: {
			register struct e_args *args2;

			arg = c_getarg(args, cst_ptyp);
			args2 = c_getarg(arg, cst_ptyp);
			args2 = c_getarg(args2, 0);
			C_exc(args->em_cst, arg->em_cst);
			break;
		}
		case ps_hol: {
			register struct e_args *args2, *args3;

			arg = c_getarg(args, cst_ptyp);
			args2 = c_getarg(arg, par_ptyp);
			args3 = c_getarg(args2, cst_ptyp);
			args3 = c_getarg(args3, 0);
			switch(arg->em_argtype) {
			    case cst_ptyp:
				C_hol_cst(args->em_cst,
					  arg->em_cst,
					  args2->em_cst);
				break;
			    case ico_ptyp:
				C_hol_icon(args->em_cst,
					   arg->em_str,
					   arg->em_size, 
					   args2->em_cst);
				break;
			    case uco_ptyp:
				C_hol_ucon(args->em_cst,
					   arg->em_str,
					   arg->em_size,
					   args2->em_cst);
				break;
			    case fco_ptyp:
				C_hol_fcon(args->em_cst,
					   arg->em_str,
					   arg->em_size, 
					   args2->em_cst);
				break;
			    case sof_ptyp:
				C_hol_dnam(args->em_cst,
					   arg->em_dnam,
					   arg->em_soff, 
					   args2->em_cst);
				break;
			    case nof_ptyp:
				C_hol_dlb(args->em_cst,
					  arg->em_dlb,
					  arg->em_noff, 
					  args2->em_cst);
				break;
			    case ilb_ptyp:
				C_hol_ilb(args->em_cst,
					  arg->em_ilb, 
					  args2->em_cst);
				break;
			    case pro_ptyp:
				C_hol_pnam(args->em_cst, 
					   arg->em_pnam, 
					   args2->em_cst);
				break;
			    default:
				EM_error = "Illegal parameter type";
				break;
			}
			break;
		}
		case ps_bss: {
			register struct e_args *args2, *args3;

			arg = c_getarg(args, cst_ptyp);
			args2 = c_getarg(arg, par_ptyp);
			args3 = c_getarg(args2, cst_ptyp);
			args3 = c_getarg(args3, 0);
			switch(arg->em_argtype) {
			    case cst_ptyp:
				C_bss_cst(args->em_cst,
					  arg->em_cst,
					  args2->em_cst);
				break;
			    case ico_ptyp:
				C_bss_icon(args->em_cst,
					   arg->em_str,
					   arg->em_size, 
					   args2->em_cst);
				break;
			    case uco_ptyp:
				C_bss_ucon(args->em_cst,
					   arg->em_str,
					   arg->em_size,
					   args2->em_cst);
				break;
			    case fco_ptyp:
				C_bss_fcon(args->em_cst,
					   arg->em_str,
					   arg->em_size, 
					   args2->em_cst);
				break;
			    case sof_ptyp:
				C_bss_dnam(args->em_cst,
					   arg->em_dnam,
					   arg->em_soff, 
					   args2->em_cst);
				break;
			    case nof_ptyp:
				C_bss_dlb(args->em_cst,
					  arg->em_dlb,
					  arg->em_noff, 
					  args2->em_cst);
				break;
			    case ilb_ptyp:
				C_bss_ilb(args->em_cst,
					  arg->em_ilb, 
					  args2->em_cst);
				break;
			    case pro_ptyp:
				C_bss_pnam(args->em_cst, 
					   arg->em_pnam, 
					   args2->em_cst);
				break;
			    default:
				EM_error = "Illegal parameter type";
				break;
			}
			break;
		}
		case ps_end:
			if (args) {
				arg = c_getarg(args, cst_ptyp);
				arg = c_getarg(arg, 0);
				C_end(args->em_cst);
				break;
			}
			C_end_narg();
			break;
		case ps_exa:
		case ps_ina:
			arg = c_getarg(args, lab_ptyp);
			arg = c_getarg(arg, 0);
			if (args->em_argtype == nof_ptyp) {
				if (opcode == ps_exa) {
					C_exa_dlb(args->em_dlb);
				}
				else	C_ina_dlb(args->em_dlb);
				break;
			}
			if (opcode == ps_exa) {
				C_exa_dnam(args->em_dnam);
			}
			else	C_ina_dnam(args->em_dnam);
			break;
		case ps_exp:
		case ps_inp:
			arg = c_getarg(args, pro_ptyp);
			arg = c_getarg(arg, 0);
			if (opcode == ps_exp) {
				C_exp(args->em_pnam);
			}
			else	C_inp(args->em_pnam);
			break;
		case ps_pro:
			arg = c_getarg(args, pro_ptyp);
			if (arg) {
				struct e_args *args2;

				args2 = c_getarg(arg, cst_ptyp);
				args2 = c_getarg(args2, 0);
				C_pro(args->em_pnam, arg->em_cst);
			}
			else	C_pro_narg(args->em_pnam);
			break;
		case ps_con:
			arg = c_getarg(args, val_ptyp);
			arg = c_getarg(arg, 0);
			switch(args->em_argtype) {
				case ilb_ptyp:
					C_con_ilb(args->em_ilb);
					break;
				case nof_ptyp:
					C_con_dlb(args->em_dlb, args->em_noff);
					break;
				case sof_ptyp:
					C_con_dnam(args->em_dnam, args->em_soff);
					break;
				case cst_ptyp:
					C_con_cst(args->em_cst);
					break;
				case pro_ptyp:
					C_con_pnam(args->em_pnam);
					break;
				case str_ptyp:
					C_con_scon(args->em_str, args->em_size);
					break;
				case ico_ptyp:
					C_con_icon(args->em_str, args->em_size);
					break;
				case uco_ptyp:
					C_con_ucon(args->em_str, args->em_size);
					break;
				case fco_ptyp:
					C_con_fcon(args->em_str, args->em_size);
					break;
				default:
					EM_error = "Illegal argument type";
					return 0;
			}
			break;
		case ps_rom:
			arg = c_getarg(args, val_ptyp);
			arg = c_getarg(arg, 0);
			switch(args->em_argtype) {
				case ilb_ptyp:
					C_rom_ilb(args->em_ilb);
					break;
				case nof_ptyp:
					C_rom_dlb(args->em_dlb, args->em_noff);
					break;
				case sof_ptyp:
					C_rom_dnam(args->em_dnam, args->em_soff);
					break;
				case cst_ptyp:
					C_rom_cst(args->em_cst);
					break;
				case pro_ptyp:
					C_rom_pnam(args->em_pnam);
					break;
				case str_ptyp:
					C_rom_scon(args->em_str, args->em_size);
					break;
				case ico_ptyp:
					C_rom_icon(args->em_str, args->em_size);
					break;
				case uco_ptyp:
					C_rom_ucon(args->em_str, args->em_size);
					break;
				case fco_ptyp:
					C_rom_fcon(args->em_str, args->em_size);
					break;
				default:
					EM_error = "Illegal argument type";
					return 0;
			}
			break;
		default: 
			EM_error = "Illegal pseudo instruction";
			break;
	}
}

PRIVATE
EM_docon(args)
	register struct e_args *args;
{
	register struct e_args *arg;

	arg = c_getarg(args, val_ptyp);
	arg = c_getarg(arg, 0);
	switch(args->em_argtype) {
		case ilb_ptyp:
			C_ilb(args->em_ilb);
			break;
		case nof_ptyp:
			C_dlb(args->em_dlb, args->em_noff);
			break;
		case sof_ptyp:
			C_dnam(args->em_dnam, args->em_soff);
			break;
		case cst_ptyp:
			C_cst(args->em_cst);
			break;
		case pro_ptyp:
			C_pnam(args->em_pnam);
			break;
		case str_ptyp:
			C_scon(args->em_str, args->em_size);
			break;
		case ico_ptyp:
			C_icon(args->em_str, args->em_size);
			break;
		case uco_ptyp:
			C_ucon(args->em_str, args->em_size);
			break;
		case fco_ptyp:
			C_fcon(args->em_str, args->em_size);
			break;
		default:
			EM_error = "Illegal argument type";
			break;
	}
}

PRIVATE
EM_dostartmes(args)
	register struct e_args *args;
{
	register struct e_args *arg;

	if (listtype) {
		EM_error = "Message not ended";
		return;
	}
	arg = c_getarg(args, cst_ptyp);
	arg = c_getarg(arg, 0);
	C_mes_begin(args->em_cst);
	listtype = ps_mes;
}

EXPORT int
EM_mkcalls(line)
	register struct e_instr *line;
{

#ifdef CHECKING
	if (listtype && line->em_type != EM_MESARG && line->em_type != EM_ENDMES) {
		EM_error = "Message not ended";
		return 0;
	}
#endif CHECKING
	EM_error = 0;
	switch(line->em_type) {
		default:
			EM_error = "Illegal EM line";
			break;
		case EM_MNEM:
			/* normal instruction */
			EM_doinstr(line->em_opcode, line->em_args);
			break;
		case EM_DEFILB:
			/* defining occurrence of an instruction label */
			C_df_ilb(line->em_deflb);
			break;
		case EM_DEFDLB:
			/* defining occurrence of a global data label */
			C_df_dlb(line->em_deflb);
			break;
		case EM_DEFDNAM:
			/* defining occurrence of a non-numeric data label */
			C_df_dnam(line->em_defdnam);
			break;
		case EM_PSEU:
			/* pseudo */
			EM_dopseudo(line->em_opcode, line->em_args);
			break;
		case EM_STARTMES:
			/* start of a MES pseudo */
			EM_dostartmes(line->em_arg);
			break;
		case EM_MESARG:
		case EM_ENDMES:
#ifdef CHECKING
			if (!listtype) {
				EM_error = "Message not started";
				return 0;
			}
#endif
			if (line->em_type == EM_MESARG) {
				EM_docon(line->em_arg);
				break;
			}
			C_mes_end();
			listtype = 0;
			break;
	}
	if (EM_error) return 0;
	return 1;
}
