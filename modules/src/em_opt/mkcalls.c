#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "nopt.h"

mkcalls(p)
	struct instr *p;
{
	switch(p->argtype) {
	case lab_ptyp:
		O_df_ilb(p->alab); break;
	case none_ptyp:
		switch(p->opcode) {
		case op_aar:
			O_aar_narg(); break;
		case op_adf:
			O_adf_narg(); break;
		case op_adi:
			O_adi_narg(); break;
		case op_ads:
			O_ads_narg(); break;
		case op_adu:
			O_adu_narg(); break;
		case op_and:
			O_and_narg(); break;
		case op_ass:
			O_ass_narg(); break;
		case op_bls:
			O_bls_narg(); break;
		case op_cmf:
			O_cmf_narg(); break;
		case op_cmi:
			O_cmi_narg(); break;
		case op_cms:
			O_cms_narg(); break;
		case op_cmu:
			O_cmu_narg(); break;
		case op_com:
			O_com_narg(); break;
		case op_csa:
			O_csa_narg(); break;
		case op_csb:
			O_csb_narg(); break;
		case op_dus:
			O_dus_narg(); break;
		case op_dvf:
			O_dvf_narg(); break;
		case op_dvi:
			O_dvi_narg(); break;
		case op_dvu:
			O_dvu_narg(); break;
		case op_exg:
			O_exg_narg(); break;
		case op_fef:
			O_fef_narg(); break;
		case op_fif:
			O_fif_narg(); break;
		case op_inn:
			O_inn_narg(); break;
		case op_ior:
			O_ior_narg(); break;
		case op_lar:
			O_lar_narg(); break;
		case op_los:
			O_los_narg(); break;
		case op_mlf:
			O_mlf_narg(); break;
		case op_mli:
			O_mli_narg(); break;
		case op_mlu:
			O_mlu_narg(); break;
		case op_ngf:
			O_ngf_narg(); break;
		case op_ngi:
			O_ngi_narg(); break;
		case op_rck:
			O_rck_narg(); break;
		case op_rmi:
			O_rmi_narg(); break;
		case op_rmu:
			O_rmu_narg(); break;
		case op_rol:
			O_rol_narg(); break;
		case op_ror:
			O_ror_narg(); break;
		case op_sar:
			O_sar_narg(); break;
		case op_sbf:
			O_sbf_narg(); break;
		case op_sbi:
			O_sbi_narg(); break;
		case op_sbs:
			O_sbs_narg(); break;
		case op_sbu:
			O_sbu_narg(); break;
		case op_set:
			O_set_narg(); break;
		case op_sli:
			O_sli_narg(); break;
		case op_slu:
			O_slu_narg(); break;
		case op_sri:
			O_sri_narg(); break;
		case op_sru:
			O_sru_narg(); break;
		case op_sts:
			O_sts_narg(); break;
		case op_xor:
			O_xor_narg(); break;
		case op_zer:
			O_zer_narg(); break;
		case op_zrf:
			O_zrf_narg(); break;
		/* no arguments */
		case op_cai:
			O_cai(); break;
		case op_cff:
			O_cff(); break;
		case op_cfi:
			O_cfi(); break;
		case op_cfu:
			O_cfu(); break;
		case op_cif:
			O_cif(); break;
		case op_cii:
			O_cii(); break;
		case op_ciu:
			O_ciu(); break;
		case op_cmp:
			O_cmp(); break;
		case op_cuf:
			O_cuf(); break;
		case op_cui:
			O_cui(); break;
		case op_cuu:
			O_cuu(); break;
		case op_dch:
			O_dch(); break;
		case op_dec:
			O_dec(); break;
		case op_inc:
			O_inc(); break;
		case op_lim:
			O_lim(); break;
		case op_lni:
			O_lni(); break;
		case op_lpb:
			O_lpb(); break;
		case op_mon:
			O_mon(); break;
		case op_nop:
			O_nop(); break;
		case op_rtt:
			O_rtt(); break;
		case op_sig:
			O_sig(); break;
		case op_sim:
			O_sim(); break;
		case op_teq:
			O_teq(); break;
		case op_tge:
			O_tge(); break;
		case op_tgt:
			O_tgt(); break;
		case op_tle:
			O_tle(); break;
		case op_tlt:
			O_tlt(); break;
		case op_tne:
			O_tne(); break;
		case op_trp:
			O_trp(); break;
		default:
			fatal("Illegal mnemonic(%d)",p->opcode);
		}
		break;
	case cst_ptyp: /* one integer constant argument */
		switch(p->opcode) {
		case op_aar:
			O_aar(p->acst); break;
		case op_adf:
			O_adf(p->acst); break;
		case op_adi:
			O_adi(p->acst); break;
		case op_adp:
			O_adp(p->acst); break;
		case op_ads:
			O_ads(p->acst); break;
		case op_adu:
			O_adu(p->acst); break;
		case op_and:
			O_and(p->acst); break;
		case op_asp:
			O_asp(p->acst); break;
		case op_ass:
			O_ass(p->acst); break;
		case op_blm:
			O_blm(p->acst); break;
		case op_bls:
			O_bls(p->acst); break;
		case op_cmf:
			O_cmf(p->acst); break;
		case op_cmi:
			O_cmi(p->acst); break;
		case op_cms:
			O_cms(p->acst); break;
		case op_cmu:
			O_cmu(p->acst); break;
		case op_com:
			O_com(p->acst); break;
		case op_csa:
			O_csa(p->acst); break;
		case op_csb:
			O_csb(p->acst); break;
		case op_del:
			O_del(p->acst); break;
		case op_dup:
			O_dup(p->acst); break;
		case op_dus:
			O_dus(p->acst); break;
		case op_dvf:
			O_dvf(p->acst); break;
		case op_dvi:
			O_dvi(p->acst); break;
		case op_dvu:
			O_dvu(p->acst); break;
		case op_exg:
			O_exg(p->acst); break;
		case op_fef:
			O_fef(p->acst); break;
		case op_fif:
			O_fif(p->acst); break;
		case op_inl:
			O_inl(p->acst); break;
		case op_inn:
			O_inn(p->acst); break;
		case op_ior:
			O_ior(p->acst); break;
		case op_lal:
			O_lal(p->acst); break;
		case op_lar:
			O_lar(p->acst); break;
		case op_ldc:
			O_ldc(p->acst); break;
		case op_ldf:
			O_ldf(p->acst); break;
		case op_ldl:
			O_ldl(p->acst); break;
		case op_lfr:
			O_lfr(p->acst); break;
		case op_lil:
			O_lil(p->acst); break;
		case op_lin:
			O_lin(p->acst); break;
		case op_loc:
			O_loc(p->acst); break;
		case op_lof:
			O_lof(p->acst); break;
		case op_loi:
			O_loi(p->acst); break;
		case op_lol:
			O_lol(p->acst); break;
		case op_lor:
			O_lor(p->acst); break;
		case op_los:
			O_los(p->acst); break;
		case op_lxa:
			O_lxa(p->acst); break;
		case op_lxl:
			O_lxl(p->acst); break;
		case op_mlf:
			O_mlf(p->acst); break;
		case op_mli:
			O_mli(p->acst); break;
		case op_mlu:
			O_mlu(p->acst); break;
		case op_ngf:
			O_ngf(p->acst); break;
		case op_ngi:
			O_ngi(p->acst); break;
		case op_rck:
			O_rck(p->acst); break;
		case op_ret:
			O_ret(p->acst); break;
		case op_rmi:
			O_rmi(p->acst); break;
		case op_rmu:
			O_rmu(p->acst); break;
		case op_rol:
			O_rol(p->acst); break;
		case op_ror:
			O_ror(p->acst); break;
		case op_sar:
			O_sar(p->acst); break;
		case op_sbf:
			O_sbf(p->acst); break;
		case op_sbi:
			O_sbi(p->acst); break;
		case op_sbs:
			O_sbs(p->acst); break;
		case op_sbu:
			O_sbu(p->acst); break;
		case op_sdf:
			O_sdf(p->acst); break;
		case op_sdl:
			O_sdl(p->acst); break;
		case op_set:
			O_set(p->acst); break;
		case op_sil:
			O_sil(p->acst); break;
		case op_sli:
			O_sli(p->acst); break;
		case op_slu:
			O_slu(p->acst); break;
		case op_sri:
			O_sri(p->acst); break;
		case op_sru:
			O_sru(p->acst); break;
		case op_stf:
			O_stf(p->acst); break;
		case op_sti:
			O_sti(p->acst); break;
		case op_stl:
			O_stl(p->acst); break;
		case op_str:
			O_str(p->acst); break;
		case op_sts:
			O_sts(p->acst); break;
		case op_xor:
			O_xor(p->acst); break;
		case op_zer:
			O_zer(p->acst); break;
		case op_zrf:
			O_zrf(p->acst); break;
		case op_zrl:
			O_zrl(p->acst); break;
		case op_beq:
			O_beq((label)p->acst); break;
		case op_bge:
			O_bge((label)p->acst); break;
		case op_bgt:
			O_bgt((label)p->acst); break;
		case op_ble:
			O_ble((label)p->acst); break;
		case op_blt:
			O_blt((label)p->acst); break;
		case op_bne:
			O_bne((label)p->acst); break;
		case op_bra:
			O_bra((label)p->acst); break;
		case op_zeq:
			O_zeq((label)p->acst); break;
		case op_zge:
			O_zge((label)p->acst); break;
		case op_zgt:
			O_zgt((label)p->acst); break;
		case op_zle:
			O_zle((label)p->acst); break;
		case op_zlt:
			O_zlt((label)p->acst); break;
		case op_zne:
			O_zne((label)p->acst); break;
		default:
			fatal("Illegal mnemonic(%d)",p->opcode);
		}
		break;
	case pro_ptyp: /* A procedure name argument */
		switch(p->opcode) {
		case op_cal:
			O_cal(p->apnam); break;
		case op_lpi:
			O_lpi(p->apnam); break;
		default:
			fatal("Illegal mnemonic(%d)",p->opcode);
		}
		break;
	case nof_ptyp: /* a "g" argument */
		switch(p->opcode) {
		case op_dee:
			O_dee_dlb(p->adlb, p->anoff); break;
		case op_fil:
			O_fil_dlb(p->adlb, p->anoff); break;
		case op_gto:
			O_gto_dlb(p->adlb, p->anoff); break;
		case op_ine:
			O_ine_dlb(p->adlb, p->anoff); break;
		case op_lae:
			O_lae_dlb(p->adlb, p->anoff); break;
		case op_lde:
			O_lde_dlb(p->adlb, p->anoff); break;
		case op_loe:
			O_loe_dlb(p->adlb, p->anoff); break;
		case op_sde:
			O_sde_dlb(p->adlb, p->anoff); break;
		case op_ste:
			O_ste_dlb(p->adlb, p->anoff); break;
		case op_zre:
			O_zre_dlb(p->adlb, p->anoff); break;
		default:
			fatal("Illegal mnemonic(%d)",p->opcode);
		}
		break;
	case sof_ptyp:
		switch(p->opcode) {
		case op_dee:
			O_dee_dnam(p->adnam, p->asoff); break;
		case op_fil:
			O_fil_dnam(p->adnam, p->asoff); break;
		case op_gto:
			O_gto_dnam(p->adnam, p->asoff); break;
		case op_ine:
			O_ine_dnam(p->adnam, p->asoff); break;
		case op_lae:
			O_lae_dnam(p->adnam, p->asoff); break;
		case op_lde:
			O_lde_dnam(p->adnam, p->asoff); break;
		case op_loe:
			O_loe_dnam(p->adnam, p->asoff); break;
		case op_sde:
			O_sde_dnam(p->adnam, p->asoff); break;
		case op_ste:
			O_ste_dnam(p->adnam, p->asoff); break;
		case op_zre:
			O_zre_dnam(p->adnam, p->asoff); break;
		default:
			fatal("Illegal mnemonic(%d)",p->opcode);
		}
		break;
	default:
		fatal("Illegal argtype(%d)",p->argtype);
	}
}
