/*
 * $Source$
 * $State$
 */

%token <y_word> GPR
%token <y_word> SPR
%token <y_word> FPR
%token <y_word> CR
%token <y_word> C
%token <y_word> OP_HI OP_HA OP_LO

%token <y_word> OP
%token <y_word> OP_BDA
%token <y_word> OP_BDL
%token <y_word> OP_BF
%token <y_word> OP_BF_BFA
%token <y_word> OP_BF_FRA_FRB
%token <y_word> OP_BF_L_RA_RB
%token <y_word> OP_BF_L_RA_SI
%token <y_word> OP_BF_L_RA_UI
%token <y_word> OP_BF_RA_RB
%token <y_word> OP_BF_RA_SI
%token <y_word> OP_BF_RA_UI
%token <y_word> OP_BF_U_C
%token <y_word> OP_BH
%token <y_word> OP_BI_BDA
%token <y_word> OP_BI_BDL
%token <y_word> OP_BI_BH
%token <y_word> OP_BICR_BDA
%token <y_word> OP_BICR_BDL
%token <y_word> OP_BICR_BH
%token <y_word> OP_BO_BI_BDA
%token <y_word> OP_BO_BI_BDL
%token <y_word> OP_BO_BI_BH
%token <y_word> OP_BT_C
%token <y_word> OP_BT_BA_BA
%token <y_word> OP_BT_BA_BB
%token <y_word> OP_BT_BT_BT
%token <y_word> OP_FLM_FRB_C
%token <y_word> OP_FRS_RA_D
%token <y_word> OP_FRS_RA_RB
%token <y_word> OP_FRT_C
%token <y_word> OP_FRT_FRA_FRB_C
%token <y_word> OP_FRT_FRA_FRC_FRB_C
%token <y_word> OP_FRT_FRA_FRC_C
%token <y_word> OP_FRT_FRB_C
%token <y_word> OP_FRT_RA_D
%token <y_word> OP_FRT_RA_RB
%token <y_word> OP_L
%token <y_word> OP_LEV
%token <y_word> OP_LIA
%token <y_word> OP_LIL
%token <y_word> OP_LI32
%token <y_word> OP_RA_RB
%token <y_word> OP_RA_RB_TH
%token <y_word> OP_RA_RS_C
%token <y_word> OP_RA_RS_RA_C
%token <y_word> OP_RA_RS_RB_C
%token <y_word> OP_RA_RS_RB_MB5_ME5_C
%token <y_word> OP_RA_RS_RB_MB6_C
%token <y_word> OP_RA_RS_SH5_C
%token <y_word> OP_RA_RS_SH5_MB5_ME5_C
%token <y_word> OP_RA_RS_SH6_C
%token <y_word> OP_RA_RS_SH6_MB6_C
%token <y_word> OP_RA_RS_UI
%token <y_word> OP_RA_RS_UI_CC
%token <y_word> OP_RS
%token <y_word> OP_RS_FXM
%token <y_word> OP_RS_RA
%token <y_word> OP_RS_RA_D
%token <y_word> OP_RS_RA_DS
%token <y_word> OP_RS_RA_NB
%token <y_word> OP_RS_RA_RB
%token <y_word> OP_RS_RA_RB_CC
%token <y_word> OP_RS_RB
%token <y_word> OP_RS_SPR
%token <y_word> OP_RS_SR
%token <y_word> OP_RT
%token <y_word> OP_RT_FXM
%token <y_word> OP_RT_RA_C
%token <y_word> OP_RT_RA_D
%token <y_word> OP_RT_RA_DS
%token <y_word> OP_RT_RA_NB
%token <y_word> OP_RT_RA_RB
%token <y_word> OP_RT_RA_RB_C
%token <y_word> OP_RT_RA_SI
%token <y_word> OP_RT_RA_SI_addic
%token <y_word> OP_RT_RA_SI_subi
%token <y_word> OP_RT_RA_SI_subic
%token <y_word> OP_RT_RB
%token <y_word> OP_RT_RB_RA_C
%token <y_word> OP_RT_SI
%token <y_word> OP_RT_SPR
%token <y_word> OP_RT_SR
%token <y_word> OP_RT_TBR
%token <y_word> OP_TH_RA_RB
%token <y_word> OP_TO_RA_RB
%token <y_word> OP_TO_RA_SI
%token <y_word> OP_TOX_RA_RB
%token <y_word> OP_TOX_RA_SI
%token <y_word> OP_clrlsldi OP_clrldi OP_clrrdi OP_extldi OP_extrdi
%token <y_word> OP_insrdi OP_rotrdi OP_sldi OP_srdi
%token <y_word> OP_clrlslwi OP_clrlwi OP_clrrwi OP_extlwi OP_extrwi
%token <y_word> OP_inslwi OP_insrwi OP_rotlwi OP_rotrwi OP_slwi OP_srwi

/* Other token types */

%type <y_word> c
%type <y_word> e16 negate16 u8 u7 u6 u5 u4 u2 u1
%type <y_word> opt_bh cr_opt nb ds bda bdl lia lil
%type <y_word> spr_num tbr_num opt_tbr
