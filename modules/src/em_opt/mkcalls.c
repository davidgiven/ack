#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "nopt.h"

OO_mkcalls(p)
	struct instr *p;
{
	switch(p->opcode) {
	case op_aar:
		if(p->argtype!=none_ptyp) O_aar(p->acst);
		else O_aar_narg(); break;
	case op_adf:
		if(p->argtype!=none_ptyp) O_adf(p->acst);
		else O_adf_narg(); break;
	case op_adi:
		if(p->argtype!=none_ptyp) O_adi(p->acst);
		else O_adi_narg(); break;
	case op_adp:
		O_adp(p->acst); break;
	case op_ads:
		if(p->argtype!=none_ptyp) O_ads(p->acst);
		else O_ads_narg(); break;
	case op_adu:
		if(p->argtype!=none_ptyp) O_adu(p->acst);
		else O_adu_narg(); break;
	case op_and:
		if(p->argtype!=none_ptyp) O_and(p->acst);
		else O_and_narg(); break;
	case op_asp:
		O_asp(p->acst); break;
	case op_ass:
		if(p->argtype!=none_ptyp) O_ass(p->acst);
		else O_ass_narg(); break;
	case op_beq:
		O_beq((label)p->acst); break;
	case op_bge:
		O_bge((label)p->acst); break;
	case op_bgt:
		O_bgt((label)p->acst); break;
	case op_ble:
		O_ble((label)p->acst); break;
	case op_blm:
		O_blm(p->acst); break;
	case op_bls:
		if(p->argtype!=none_ptyp) O_bls(p->acst);
		else O_bls_narg(); break;
	case op_blt:
		O_blt((label)p->acst); break;
	case op_bne:
		O_bne((label)p->acst); break;
	case op_bra:
		O_bra((label)p->acst); break;
	case op_cai:
		O_cai(); break;
	case op_cal:
		O_cal(p->apnam); break;
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
	case op_cmf:
		if(p->argtype!=none_ptyp) O_cmf(p->acst);
		else O_cmf_narg(); break;
	case op_cmi:
		if(p->argtype!=none_ptyp) O_cmi(p->acst);
		else O_cmi_narg(); break;
	case op_cmp:
		O_cmp(); break;
	case op_cms:
		if(p->argtype!=none_ptyp) O_cms(p->acst);
		else O_cms_narg(); break;
	case op_cmu:
		if(p->argtype!=none_ptyp) O_cmu(p->acst);
		else O_cmu_narg(); break;
	case op_com:
		if(p->argtype!=none_ptyp) O_com(p->acst);
		else O_com_narg(); break;
	case op_csa:
		if(p->argtype!=none_ptyp) O_csa(p->acst);
		else O_csa_narg(); break;
	case op_csb:
		if(p->argtype!=none_ptyp) O_csb(p->acst);
		else O_csb_narg(); break;
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
	case op_dee:
		if(p->argtype==nof_ptyp) O_dee_dlb(p->adlb, p->anoff);
		else O_dee_dnam(p->adnam, p->asoff); break;
	case op_del:
		O_del(p->acst); break;
	case op_dup:
		O_dup(p->acst); break;
	case op_dus:
		if(p->argtype!=none_ptyp) O_dus(p->acst);
		else O_dus_narg(); break;
	case op_dvf:
		if(p->argtype!=none_ptyp) O_dvf(p->acst);
		else O_dvf_narg(); break;
	case op_dvi:
		if(p->argtype!=none_ptyp) O_dvi(p->acst);
		else O_dvi_narg(); break;
	case op_dvu:
		if(p->argtype!=none_ptyp) O_dvu(p->acst);
		else O_dvu_narg(); break;
	case op_exg:
		if(p->argtype!=none_ptyp) O_exg(p->acst);
		else O_exg_narg(); break;
	case op_fef:
		if(p->argtype!=none_ptyp) O_fef(p->acst);
		else O_fef_narg(); break;
	case op_fif:
		if(p->argtype!=none_ptyp) O_fif(p->acst);
		else O_fif_narg(); break;
	case op_fil:
		if(p->argtype==nof_ptyp) O_fil_dlb(p->adlb, p->anoff);
		else O_fil_dnam(p->adnam, p->asoff); break;
	case op_gto:
		if(p->argtype==nof_ptyp) O_gto_dlb(p->adlb, p->anoff);
		else O_gto_dnam(p->adnam, p->asoff); break;
	case op_inc:
		O_inc(); break;
	case op_ine:
		if(p->argtype==nof_ptyp) O_ine_dlb(p->adlb, p->anoff);
		else O_ine_dnam(p->adnam, p->asoff); break;
	case op_inl:
		O_inl(p->acst); break;
	case op_inn:
		if(p->argtype!=none_ptyp) O_inn(p->acst);
		else O_inn_narg(); break;
	case op_ior:
		if(p->argtype!=none_ptyp) O_ior(p->acst);
		else O_ior_narg(); break;
	case op_lab:
		O_df_ilb(p->alab); break;
	case op_lae:
		if(p->argtype==nof_ptyp) O_lae_dlb(p->adlb, p->anoff);
		else O_lae_dnam(p->adnam, p->asoff); break;
	case op_lal:
		O_lal(p->acst); break;
	case op_lar:
		if(p->argtype!=none_ptyp) O_lar(p->acst);
		else O_lar_narg(); break;
	case op_ldc:
		O_ldc(p->acst); break;
	case op_lde:
		if(p->argtype==nof_ptyp) O_lde_dlb(p->adlb, p->anoff);
		else O_lde_dnam(p->adnam, p->asoff); break;
	case op_ldf:
		O_ldf(p->acst); break;
	case op_ldl:
		O_ldl(p->acst); break;
	case op_lfr:
		O_lfr(p->acst); break;
	case op_lil:
		O_lil(p->acst); break;
	case op_lim:
		O_lim(); break;
	case op_lin:
		O_lin(p->acst); break;
	case op_lni:
		O_lni(); break;
	case op_loc:
		O_loc(p->acst); break;
	case op_loe:
		if(p->argtype==nof_ptyp) O_loe_dlb(p->adlb, p->anoff);
		else O_loe_dnam(p->adnam, p->asoff); break;
	case op_lof:
		O_lof(p->acst); break;
	case op_loi:
		O_loi(p->acst); break;
	case op_lol:
		O_lol(p->acst); break;
	case op_lor:
		O_lor(p->acst); break;
	case op_los:
		if(p->argtype!=none_ptyp) O_los(p->acst);
		else O_los_narg(); break;
	case op_lpb:
		O_lpb(); break;
	case op_lpi:
		O_lpi(p->apnam); break;
	case op_lxa:
		O_lxa(p->acst); break;
	case op_lxl:
		O_lxl(p->acst); break;
	case op_mlf:
		if(p->argtype!=none_ptyp) O_mlf(p->acst);
		else O_mlf_narg(); break;
	case op_mli:
		if(p->argtype!=none_ptyp) O_mli(p->acst);
		else O_mli_narg(); break;
	case op_mlu:
		if(p->argtype!=none_ptyp) O_mlu(p->acst);
		else O_mlu_narg(); break;
	case op_mon:
		O_mon(); break;
	case op_ngf:
		if(p->argtype!=none_ptyp) O_ngf(p->acst);
		else O_ngf_narg(); break;
	case op_ngi:
		if(p->argtype!=none_ptyp) O_ngi(p->acst);
		else O_ngi_narg(); break;
	case op_nop:
		O_nop(); break;
	case op_rck:
		if(p->argtype!=none_ptyp) O_rck(p->acst);
		else O_rck_narg(); break;
	case op_ret:
		O_ret(p->acst); break;
	case op_rmi:
		if(p->argtype!=none_ptyp) O_rmi(p->acst);
		else O_rmi_narg(); break;
	case op_rmu:
		if(p->argtype!=none_ptyp) O_rmu(p->acst);
		else O_rmu_narg(); break;
	case op_rol:
		if(p->argtype!=none_ptyp) O_rol(p->acst);
		else O_rol_narg(); break;
	case op_ror:
		if(p->argtype!=none_ptyp) O_ror(p->acst);
		else O_ror_narg(); break;
	case op_rtt:
		O_rtt(); break;
	case op_sar:
		if(p->argtype!=none_ptyp) O_sar(p->acst);
		else O_sar_narg(); break;
	case op_sbf:
		if(p->argtype!=none_ptyp) O_sbf(p->acst);
		else O_sbf_narg(); break;
	case op_sbi:
		if(p->argtype!=none_ptyp) O_sbi(p->acst);
		else O_sbi_narg(); break;
	case op_sbs:
		if(p->argtype!=none_ptyp) O_sbs(p->acst);
		else O_sbs_narg(); break;
	case op_sbu:
		if(p->argtype!=none_ptyp) O_sbu(p->acst);
		else O_sbu_narg(); break;
	case op_sde:
		if(p->argtype==nof_ptyp) O_sde_dlb(p->adlb, p->anoff);
		else O_sde_dnam(p->adnam, p->asoff); break;
	case op_sdf:
		O_sdf(p->acst); break;
	case op_sdl:
		O_sdl(p->acst); break;
	case op_set:
		if(p->argtype!=none_ptyp) O_set(p->acst);
		else O_set_narg(); break;
	case op_sig:
		O_sig(); break;
	case op_sil:
		O_sil(p->acst); break;
	case op_sim:
		O_sim(); break;
	case op_sli:
		if(p->argtype!=none_ptyp) O_sli(p->acst);
		else O_sli_narg(); break;
	case op_slu:
		if(p->argtype!=none_ptyp) O_slu(p->acst);
		else O_slu_narg(); break;
	case op_sri:
		if(p->argtype!=none_ptyp) O_sri(p->acst);
		else O_sri_narg(); break;
	case op_sru:
		if(p->argtype!=none_ptyp) O_sru(p->acst);
		else O_sru_narg(); break;
	case op_ste:
		if(p->argtype==nof_ptyp) O_ste_dlb(p->adlb, p->anoff);
		else O_ste_dnam(p->adnam, p->asoff); break;
	case op_stf:
		O_stf(p->acst); break;
	case op_sti:
		O_sti(p->acst); break;
	case op_stl:
		O_stl(p->acst); break;
	case op_str:
		O_str(p->acst); break;
	case op_sts:
		if(p->argtype!=none_ptyp) O_sts(p->acst);
		else O_sts_narg(); break;
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
	case op_xor:
		if(p->argtype!=none_ptyp) O_xor(p->acst);
		else O_xor_narg(); break;
	case op_zeq:
		O_zeq((label)p->acst); break;
	case op_zer:
		if(p->argtype!=none_ptyp) O_zer(p->acst);
		else O_zer_narg(); break;
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
	case op_zre:
		if(p->argtype==nof_ptyp) O_zre_dlb(p->adlb, p->anoff);
		else O_zre_dnam(p->adnam, p->asoff); break;
	case op_zrf:
		if(p->argtype!=none_ptyp) O_zrf(p->acst);
		else O_zrf_narg(); break;
	case op_zrl:
		O_zrl(p->acst); break;
	}
}
