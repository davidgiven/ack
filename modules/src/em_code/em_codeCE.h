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

void C_insertpart( int id);
void C_beginpart( int id);
void C_endpart( int id);
void C_pro( char * s, arith l);
void C_pro_narg( char * s);
void C_end( arith l);
void C_end_narg(void);
void C_df_dlb( label l);
void C_df_dnam( char * s);
void C_exa_dnam( char * s);
void C_exa_dlb( label l);
void C_exp( char * s);
void C_ina_dnam( char * s);
void C_ina_dlb( label l);
void C_inp( char * s);
void C_bss_cst( arith n, arith w, int i);
void C_bss_icon( arith n, char* s, arith sz, int i);
void C_bss_ucon( arith n, char* s, arith sz, int i);
void C_bss_fcon( arith n, char* s, arith sz, int i);
void C_bss_dnam( arith n, char* s, arith offs, int i);
void C_bss_dlb( arith n, label l, arith offs, int i);
void C_bss_ilb( arith n, label l, int i);
void C_bss_pnam( arith n, char* s, int i);
void C_hol_cst( arith n, arith w, int i);
void C_hol_icon( arith n, char* s, arith sz, int i);
void C_hol_ucon( arith n, char* s, arith sz, int i);
void C_hol_fcon( arith n, char* s, arith sz, int i);
void C_hol_dnam( arith n, char* s, arith offs, int i);
void C_hol_dlb( arith n, label l, arith offs, int i);
void C_hol_ilb( arith n, label l, int i);
void C_hol_pnam( arith n, char* s, int i);
void C_con_cst( arith l);
void C_con_icon( char * val, arith siz);
void C_con_ucon( char * val, arith siz);
void C_con_fcon( char * val, arith siz);
void C_con_scon( char * str, arith siz);
void C_con_dnam( char * str, arith val);
void C_con_dlb( label l, arith val);
void C_con_ilb( label l);
void C_con_pnam( char * str);
void C_rom_cst( arith l);
void C_rom_icon( char * val, arith siz);
void C_rom_ucon( char * val, arith siz);
void C_rom_fcon( char * val, arith siz);
void C_rom_scon( char * str, arith siz);
void C_rom_dnam( char * str, arith val);
void C_rom_dlb( label l, arith val);
void C_rom_ilb( label l);
void C_rom_pnam( char * str);
void C_cst( arith l);
void C_icon( char * val, arith siz);
void C_ucon( char * val, arith siz);
void C_fcon( char * val, arith siz);
void C_scon( char * str, arith siz);
void C_dnam( char * str, arith val);
void C_dlb( label l, arith val);
void C_ilb( label l);
void C_pnam( char * str);
void C_mes_begin( int ms);
void C_mes_end(void);
void C_exc( arith c1, arith c2);
void C_zrl( arith n);
void C_zrf_narg(void);
void C_zrf( arith n);
void C_zre( arith n);
void C_zre_dnam( char * s, arith n);
void C_zre_dlb( label l, arith n);
void C_zne( label l);
void C_zlt( label l);
void C_zle( label l);
void C_zgt( label l);
void C_zge( label l);
void C_zer_narg(void);
void C_zer( arith n);
void C_zeq( label l);
void C_xor_narg(void);
void C_xor( arith n);
void C_trp(void);
void C_tne(void);
void C_tlt(void);
void C_tle(void);
void C_tgt(void);
void C_tge(void);
void C_teq(void);
void C_sts_narg(void);
void C_sts( arith n);
void C_str( arith n);
void C_stl( arith n);
void C_sti( arith n);
void C_stf( arith n);
void C_ste( arith n);
void C_ste_dnam( char * s, arith n);
void C_ste_dlb( label l, arith n);
void C_sru_narg(void);
void C_sru( arith n);
void C_sri_narg(void);
void C_sri( arith n);
void C_slu_narg(void);
void C_slu( arith n);
void C_sli_narg(void);
void C_sli( arith n);
void C_sim(void);
void C_sil( arith n);
void C_sig(void);
void C_set_narg(void);
void C_set( arith n);
void C_sdl( arith n);
void C_sdf( arith n);
void C_sde( arith n);
void C_sde_dnam( char * s, arith n);
void C_sde_dlb( label l, arith n);
void C_sbu_narg(void);
void C_sbu( arith n);
void C_sbs_narg(void);
void C_sbs( arith n);
void C_sbi_narg(void);
void C_sbi( arith n);
void C_sbf_narg(void);
void C_sbf( arith n);
void C_sar_narg(void);
void C_sar( arith n);
void C_rtt(void);
void C_ror_narg(void);
void C_ror( arith n);
void C_rol_narg(void);
void C_rol( arith n);
void C_rmu_narg(void);
void C_rmu( arith n);
void C_rmi_narg(void);
void C_rmi( arith n);
void C_ret( arith n);
void C_rck_narg(void);
void C_rck( arith n);
void C_nop(void);
void C_ngi_narg(void);
void C_ngi( arith n);
void C_ngf_narg(void);
void C_ngf( arith n);
void C_mon(void);
void C_mlu_narg(void);
void C_mlu( arith n);
void C_mli_narg(void);
void C_mli( arith n);
void C_mlf_narg(void);
void C_mlf( arith n);
void C_lxl( arith n);
void C_lxa( arith n);
void C_lpi( char * s);
void C_lpb(void);
void C_los_narg(void);
void C_los( arith n);
void C_lor( arith n);
void C_lol( arith n);
void C_loi( arith n);
void C_lof( arith n);
void C_loe( arith n);
void C_loe_dnam( char * s, arith n);
void C_loe_dlb( label l, arith n);
void C_loc( arith n);
void C_lni(void);
void C_lin( arith n);
void C_lim(void);
void C_lil( arith n);
void C_lfr( arith n);
void C_ldl( arith n);
void C_ldf( arith n);
void C_lde( arith n);
void C_lde_dnam( char * s, arith n);
void C_lde_dlb( label l, arith n);
void C_ldc( arith n);
void C_lar_narg(void);
void C_lar( arith n);
void C_lal( arith n);
void C_lae( arith n);
void C_lae_dnam( char * s, arith n);
void C_lae_dlb( label l, arith n);
void C_ior_narg(void);
void C_ior( arith n);
void C_inn_narg(void);
void C_inn( arith n);
void C_inl( arith n);
void C_ine( arith n);
void C_ine_dnam( char * s, arith n);
void C_ine_dlb( label l, arith n);
void C_inc(void);
void C_gto( arith n);
void C_gto_dnam( char * s, arith n);
void C_gto_dlb( label l, arith n);
void C_fil( arith n);
void C_fil_dnam( char * s, arith n);
void C_fil_dlb( label l, arith n);
void C_fif_narg(void);
void C_fif( arith n);
void C_fef_narg(void);
void C_fef( arith n);
void C_exg_narg(void);
void C_exg( arith n);
void C_dvu_narg(void);
void C_dvu( arith n);
void C_dvi_narg(void);
void C_dvi( arith n);
void C_dvf_narg(void);
void C_dvf( arith n);
void C_dus_narg(void);
void C_dus( arith n);
void C_dup( arith n);
void C_del( arith n);
void C_dee( arith n);
void C_dee_dnam( char * s, arith n);
void C_dee_dlb( label l, arith n);
void C_dec(void);
void C_dch(void);
void C_cuu(void);
void C_cui(void);
void C_cuf(void);
void C_csb_narg(void);
void C_csb( arith n);
void C_csa_narg(void);
void C_csa( arith n);
void C_com_narg(void);
void C_com( arith n);
void C_cmu_narg(void);
void C_cmu( arith n);
void C_cms_narg(void);
void C_cms( arith n);
void C_cmp(void);
void C_cmi_narg(void);
void C_cmi( arith n);
void C_cmf_narg(void);
void C_cmf( arith n);
void C_ciu(void);
void C_cii(void);
void C_cif(void);
void C_cfu(void);
void C_cfi(void);
void C_cff(void);
void C_cal( char * s);
void C_cai(void);
void C_bra( label l);
void C_bne( label l);
void C_blt( label l);
void C_bls_narg(void);
void C_bls( arith n);
void C_blm( arith n);
void C_ble( label l);
void C_bgt( label l);
void C_bge( label l);
void C_beq( label l);
void C_ass_narg(void);
void C_ass( arith n);
void C_asp( arith n);
void C_and_narg(void);
void C_and( arith n);
void C_adu_narg(void);
void C_adu( arith n);
void C_ads_narg(void);
void C_ads( arith n);
void C_adp( arith n);
void C_adi_narg(void);
void C_adi( arith n);
void C_adf_narg(void);
void C_adf( arith n);
void C_aar_narg(void);
void C_aar( arith n);
void C_df_ilb( label l);
