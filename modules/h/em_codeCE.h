#define C_lae_dnam _1_C_lae_dnam
#define C_lae_dlb _2_C_lae_dlb
#define C_sde_dnam _3_C_sde_dnam
#define C_sde_dlb _4_C_sde_dlb
#define C_zre_dnam _5_C_zre_dnam
#define C_zre_dlb _6_C_zre_dlb
#define C_hol_dnam _7_C_hol_dnam
#define C_hol_dlb _8_C_hol_dlb
#define C_hol_icon _9_C_hol_icon
#define C_hol_ilb _10_C_hol_ilb
#define C_bss_dnam _11_C_bss_dnam
#define C_bss_dlb _12_C_bss_dlb
#define C_bss_icon _13_C_bss_icon
#define C_bss_ilb _14_C_bss_ilb
#define C_con_dnam _15_C_con_dnam
#define C_con_dlb _16_C_con_dlb
#define C_con_icon _17_C_con_icon
#define C_con_ilb _18_C_con_ilb
#define C_gto_dnam _19_C_gto_dnam
#define C_gto_dlb _20_C_gto_dlb
#define C_ine_dnam _21_C_ine_dnam
#define C_ine_dlb _22_C_ine_dlb
#define C_rom_dnam _23_C_rom_dnam
#define C_rom_dlb _24_C_rom_dlb
#define C_ina_dnam _25_C_ina_dnam
#define C_ina_dlb _26_C_ina_dlb
#define C_rom_icon _27_C_rom_icon
#define C_rom_ilb _28_C_rom_ilb
#define C_ste_dnam _29_C_ste_dnam
#define C_ste_dlb _30_C_ste_dlb
#define C_fil_dnam _31_C_fil_dnam
#define C_fil_dlb _32_C_fil_dlb
#define C_lde_dnam _33_C_lde_dnam
#define C_lde_dlb _34_C_lde_dlb
#define C_loe_dnam _35_C_loe_dnam
#define C_loe_dlb _36_C_loe_dlb
#define C_exa_dnam _37_C_exa_dnam
#define C_exa_dlb _38_C_exa_dlb
#define C_dee_dnam _39_C_dee_dnam
#define C_dee_dlb _40_C_dee_dlb

_PROTOTYPE(void C_insertpart, ( int id));
_PROTOTYPE(void C_beginpart, ( int id));
_PROTOTYPE(void C_endpart, ( int id));
_PROTOTYPE(void C_pro, ( char * s, arith l));
_PROTOTYPE(void C_pro_narg, ( char * s));
_PROTOTYPE(void C_end, ( arith l));
_PROTOTYPE(void C_end_narg, (void));
_PROTOTYPE(void C_df_dlb, ( label l));
_PROTOTYPE(void C_df_dnam, ( char * s));
_PROTOTYPE(void C_exa_dnam, ( char * s));
_PROTOTYPE(void C_exa_dlb, ( label l));
_PROTOTYPE(void C_exp, ( char * s));
_PROTOTYPE(void C_ina_dnam, ( char * s));
_PROTOTYPE(void C_ina_dlb, ( label l));
_PROTOTYPE(void C_inp, ( char * s));
_PROTOTYPE(void C_bss_cst, ( arith n, arith w, int i));
_PROTOTYPE(void C_bss_icon, ( arith n, char* s, arith sz, int i));
_PROTOTYPE(void C_bss_ucon, ( arith n, char* s, arith sz, int i));
_PROTOTYPE(void C_bss_fcon, ( arith n, char* s, arith sz, int i));
_PROTOTYPE(void C_bss_dnam, ( arith n, char* s, arith offs, int i));
_PROTOTYPE(void C_bss_dlb, ( arith n, label l, arith offs, int i));
_PROTOTYPE(void C_bss_ilb, ( arith n, label l, int i));
_PROTOTYPE(void C_bss_pnam, ( arith n, char* s, int i));
_PROTOTYPE(void C_hol_cst, ( arith n, arith w, int i));
_PROTOTYPE(void C_hol_icon, ( arith n, char* s, arith sz, int i));
_PROTOTYPE(void C_hol_ucon, ( arith n, char* s, arith sz, int i));
_PROTOTYPE(void C_hol_fcon, ( arith n, char* s, arith sz, int i));
_PROTOTYPE(void C_hol_dnam, ( arith n, char* s, arith offs, int i));
_PROTOTYPE(void C_hol_dlb, ( arith n, label l, arith offs, int i));
_PROTOTYPE(void C_hol_ilb, ( arith n, label l, int i));
_PROTOTYPE(void C_hol_pnam, ( arith n, char* s, int i));
_PROTOTYPE(void C_con_cst, ( arith l));
_PROTOTYPE(void C_con_icon, ( char * val, arith siz));
_PROTOTYPE(void C_con_ucon, ( char * val, arith siz));
_PROTOTYPE(void C_con_fcon, ( char * val, arith siz));
_PROTOTYPE(void C_con_scon, ( char * str, arith siz));
_PROTOTYPE(void C_con_dnam, ( char * str, arith val));
_PROTOTYPE(void C_con_dlb, ( label l, arith val));
_PROTOTYPE(void C_con_ilb, ( label l));
_PROTOTYPE(void C_con_pnam, ( char * str));
_PROTOTYPE(void C_rom_cst, ( arith l));
_PROTOTYPE(void C_rom_icon, ( char * val, arith siz));
_PROTOTYPE(void C_rom_ucon, ( char * val, arith siz));
_PROTOTYPE(void C_rom_fcon, ( char * val, arith siz));
_PROTOTYPE(void C_rom_scon, ( char * str, arith siz));
_PROTOTYPE(void C_rom_dnam, ( char * str, arith val));
_PROTOTYPE(void C_rom_dlb, ( label l, arith val));
_PROTOTYPE(void C_rom_ilb, ( label l));
_PROTOTYPE(void C_rom_pnam, ( char * str));
_PROTOTYPE(void C_cst, ( arith l));
_PROTOTYPE(void C_icon, ( char * val, arith siz));
_PROTOTYPE(void C_ucon, ( char * val, arith siz));
_PROTOTYPE(void C_fcon, ( char * val, arith siz));
_PROTOTYPE(void C_scon, ( char * str, arith siz));
_PROTOTYPE(void C_dnam, ( char * str, arith val));
_PROTOTYPE(void C_dlb, ( label l, arith val));
_PROTOTYPE(void C_ilb, ( label l));
_PROTOTYPE(void C_pnam, ( char * str));
_PROTOTYPE(void C_mes_begin, ( int ms));
_PROTOTYPE(void C_mes_end, (void));
_PROTOTYPE(void C_exc, ( arith c1, arith c2));
_PROTOTYPE(void C_zrl, ( arith n));
_PROTOTYPE(void C_zrf_narg, (void));
_PROTOTYPE(void C_zrf, ( arith n));
_PROTOTYPE(void C_zre, ( arith n));
_PROTOTYPE(void C_zre_dnam, ( char * s, arith n));
_PROTOTYPE(void C_zre_dlb, ( label l, arith n));
_PROTOTYPE(void C_zne, ( label l));
_PROTOTYPE(void C_zlt, ( label l));
_PROTOTYPE(void C_zle, ( label l));
_PROTOTYPE(void C_zgt, ( label l));
_PROTOTYPE(void C_zge, ( label l));
_PROTOTYPE(void C_zer_narg, (void));
_PROTOTYPE(void C_zer, ( arith n));
_PROTOTYPE(void C_zeq, ( label l));
_PROTOTYPE(void C_xor_narg, (void));
_PROTOTYPE(void C_xor, ( arith n));
_PROTOTYPE(void C_trp, (void));
_PROTOTYPE(void C_tne, (void));
_PROTOTYPE(void C_tlt, (void));
_PROTOTYPE(void C_tle, (void));
_PROTOTYPE(void C_tgt, (void));
_PROTOTYPE(void C_tge, (void));
_PROTOTYPE(void C_teq, (void));
_PROTOTYPE(void C_sts_narg, (void));
_PROTOTYPE(void C_sts, ( arith n));
_PROTOTYPE(void C_str, ( arith n));
_PROTOTYPE(void C_stl, ( arith n));
_PROTOTYPE(void C_sti, ( arith n));
_PROTOTYPE(void C_stf, ( arith n));
_PROTOTYPE(void C_ste, ( arith n));
_PROTOTYPE(void C_ste_dnam, ( char * s, arith n));
_PROTOTYPE(void C_ste_dlb, ( label l, arith n));
_PROTOTYPE(void C_sru_narg, (void));
_PROTOTYPE(void C_sru, ( arith n));
_PROTOTYPE(void C_sri_narg, (void));
_PROTOTYPE(void C_sri, ( arith n));
_PROTOTYPE(void C_slu_narg, (void));
_PROTOTYPE(void C_slu, ( arith n));
_PROTOTYPE(void C_sli_narg, (void));
_PROTOTYPE(void C_sli, ( arith n));
_PROTOTYPE(void C_sim, (void));
_PROTOTYPE(void C_sil, ( arith n));
_PROTOTYPE(void C_sig, (void));
_PROTOTYPE(void C_set_narg, (void));
_PROTOTYPE(void C_set, ( arith n));
_PROTOTYPE(void C_sdl, ( arith n));
_PROTOTYPE(void C_sdf, ( arith n));
_PROTOTYPE(void C_sde, ( arith n));
_PROTOTYPE(void C_sde_dnam, ( char * s, arith n));
_PROTOTYPE(void C_sde_dlb, ( label l, arith n));
_PROTOTYPE(void C_sbu_narg, (void));
_PROTOTYPE(void C_sbu, ( arith n));
_PROTOTYPE(void C_sbs_narg, (void));
_PROTOTYPE(void C_sbs, ( arith n));
_PROTOTYPE(void C_sbi_narg, (void));
_PROTOTYPE(void C_sbi, ( arith n));
_PROTOTYPE(void C_sbf_narg, (void));
_PROTOTYPE(void C_sbf, ( arith n));
_PROTOTYPE(void C_sar_narg, (void));
_PROTOTYPE(void C_sar, ( arith n));
_PROTOTYPE(void C_rtt, (void));
_PROTOTYPE(void C_ror_narg, (void));
_PROTOTYPE(void C_ror, ( arith n));
_PROTOTYPE(void C_rol_narg, (void));
_PROTOTYPE(void C_rol, ( arith n));
_PROTOTYPE(void C_rmu_narg, (void));
_PROTOTYPE(void C_rmu, ( arith n));
_PROTOTYPE(void C_rmi_narg, (void));
_PROTOTYPE(void C_rmi, ( arith n));
_PROTOTYPE(void C_ret, ( arith n));
_PROTOTYPE(void C_rck_narg, (void));
_PROTOTYPE(void C_rck, ( arith n));
_PROTOTYPE(void C_nop, (void));
_PROTOTYPE(void C_ngi_narg, (void));
_PROTOTYPE(void C_ngi, ( arith n));
_PROTOTYPE(void C_ngf_narg, (void));
_PROTOTYPE(void C_ngf, ( arith n));
_PROTOTYPE(void C_mon, (void));
_PROTOTYPE(void C_mlu_narg, (void));
_PROTOTYPE(void C_mlu, ( arith n));
_PROTOTYPE(void C_mli_narg, (void));
_PROTOTYPE(void C_mli, ( arith n));
_PROTOTYPE(void C_mlf_narg, (void));
_PROTOTYPE(void C_mlf, ( arith n));
_PROTOTYPE(void C_lxl, ( arith n));
_PROTOTYPE(void C_lxa, ( arith n));
_PROTOTYPE(void C_lpi, ( char * s));
_PROTOTYPE(void C_lpb, (void));
_PROTOTYPE(void C_los_narg, (void));
_PROTOTYPE(void C_los, ( arith n));
_PROTOTYPE(void C_lor, ( arith n));
_PROTOTYPE(void C_lol, ( arith n));
_PROTOTYPE(void C_loi, ( arith n));
_PROTOTYPE(void C_lof, ( arith n));
_PROTOTYPE(void C_loe, ( arith n));
_PROTOTYPE(void C_loe_dnam, ( char * s, arith n));
_PROTOTYPE(void C_loe_dlb, ( label l, arith n));
_PROTOTYPE(void C_loc, ( arith n));
_PROTOTYPE(void C_lni, (void));
_PROTOTYPE(void C_lin, ( arith n));
_PROTOTYPE(void C_lim, (void));
_PROTOTYPE(void C_lil, ( arith n));
_PROTOTYPE(void C_lfr, ( arith n));
_PROTOTYPE(void C_ldl, ( arith n));
_PROTOTYPE(void C_ldf, ( arith n));
_PROTOTYPE(void C_lde, ( arith n));
_PROTOTYPE(void C_lde_dnam, ( char * s, arith n));
_PROTOTYPE(void C_lde_dlb, ( label l, arith n));
_PROTOTYPE(void C_ldc, ( arith n));
_PROTOTYPE(void C_lar_narg, (void));
_PROTOTYPE(void C_lar, ( arith n));
_PROTOTYPE(void C_lal, ( arith n));
_PROTOTYPE(void C_lae, ( arith n));
_PROTOTYPE(void C_lae_dnam, ( char * s, arith n));
_PROTOTYPE(void C_lae_dlb, ( label l, arith n));
_PROTOTYPE(void C_ior_narg, (void));
_PROTOTYPE(void C_ior, ( arith n));
_PROTOTYPE(void C_inn_narg, (void));
_PROTOTYPE(void C_inn, ( arith n));
_PROTOTYPE(void C_inl, ( arith n));
_PROTOTYPE(void C_ine, ( arith n));
_PROTOTYPE(void C_ine_dnam, ( char * s, arith n));
_PROTOTYPE(void C_ine_dlb, ( label l, arith n));
_PROTOTYPE(void C_inc, (void));
_PROTOTYPE(void C_gto, ( arith n));
_PROTOTYPE(void C_gto_dnam, ( char * s, arith n));
_PROTOTYPE(void C_gto_dlb, ( label l, arith n));
_PROTOTYPE(void C_fil, ( arith n));
_PROTOTYPE(void C_fil_dnam, ( char * s, arith n));
_PROTOTYPE(void C_fil_dlb, ( label l, arith n));
_PROTOTYPE(void C_fif_narg, (void));
_PROTOTYPE(void C_fif, ( arith n));
_PROTOTYPE(void C_fef_narg, (void));
_PROTOTYPE(void C_fef, ( arith n));
_PROTOTYPE(void C_exg_narg, (void));
_PROTOTYPE(void C_exg, ( arith n));
_PROTOTYPE(void C_dvu_narg, (void));
_PROTOTYPE(void C_dvu, ( arith n));
_PROTOTYPE(void C_dvi_narg, (void));
_PROTOTYPE(void C_dvi, ( arith n));
_PROTOTYPE(void C_dvf_narg, (void));
_PROTOTYPE(void C_dvf, ( arith n));
_PROTOTYPE(void C_dus_narg, (void));
_PROTOTYPE(void C_dus, ( arith n));
_PROTOTYPE(void C_dup, ( arith n));
_PROTOTYPE(void C_del, ( arith n));
_PROTOTYPE(void C_dee, ( arith n));
_PROTOTYPE(void C_dee_dnam, ( char * s, arith n));
_PROTOTYPE(void C_dee_dlb, ( label l, arith n));
_PROTOTYPE(void C_dec, (void));
_PROTOTYPE(void C_dch, (void));
_PROTOTYPE(void C_cuu, (void));
_PROTOTYPE(void C_cui, (void));
_PROTOTYPE(void C_cuf, (void));
_PROTOTYPE(void C_csb_narg, (void));
_PROTOTYPE(void C_csb, ( arith n));
_PROTOTYPE(void C_csa_narg, (void));
_PROTOTYPE(void C_csa, ( arith n));
_PROTOTYPE(void C_com_narg, (void));
_PROTOTYPE(void C_com, ( arith n));
_PROTOTYPE(void C_cmu_narg, (void));
_PROTOTYPE(void C_cmu, ( arith n));
_PROTOTYPE(void C_cms_narg, (void));
_PROTOTYPE(void C_cms, ( arith n));
_PROTOTYPE(void C_cmp, (void));
_PROTOTYPE(void C_cmi_narg, (void));
_PROTOTYPE(void C_cmi, ( arith n));
_PROTOTYPE(void C_cmf_narg, (void));
_PROTOTYPE(void C_cmf, ( arith n));
_PROTOTYPE(void C_ciu, (void));
_PROTOTYPE(void C_cii, (void));
_PROTOTYPE(void C_cif, (void));
_PROTOTYPE(void C_cfu, (void));
_PROTOTYPE(void C_cfi, (void));
_PROTOTYPE(void C_cff, (void));
_PROTOTYPE(void C_cal, ( char * s));
_PROTOTYPE(void C_cai, (void));
_PROTOTYPE(void C_bra, ( label l));
_PROTOTYPE(void C_bne, ( label l));
_PROTOTYPE(void C_blt, ( label l));
_PROTOTYPE(void C_bls_narg, (void));
_PROTOTYPE(void C_bls, ( arith n));
_PROTOTYPE(void C_blm, ( arith n));
_PROTOTYPE(void C_ble, ( label l));
_PROTOTYPE(void C_bgt, ( label l));
_PROTOTYPE(void C_bge, ( label l));
_PROTOTYPE(void C_beq, ( label l));
_PROTOTYPE(void C_ass_narg, (void));
_PROTOTYPE(void C_ass, ( arith n));
_PROTOTYPE(void C_asp, ( arith n));
_PROTOTYPE(void C_and_narg, (void));
_PROTOTYPE(void C_and, ( arith n));
_PROTOTYPE(void C_adu_narg, (void));
_PROTOTYPE(void C_adu, ( arith n));
_PROTOTYPE(void C_ads_narg, (void));
_PROTOTYPE(void C_ads, ( arith n));
_PROTOTYPE(void C_adp, ( arith n));
_PROTOTYPE(void C_adi_narg, (void));
_PROTOTYPE(void C_adi, ( arith n));
_PROTOTYPE(void C_adf_narg, (void));
_PROTOTYPE(void C_adf, ( arith n));
_PROTOTYPE(void C_aar_narg, (void));
_PROTOTYPE(void C_aar, ( arith n));
_PROTOTYPE(void C_df_ilb, ( label l));
