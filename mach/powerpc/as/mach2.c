/*
 * $Source$
 * $State$
 */

%token <y_word> GPR
%token <y_word> SPR
%token <y_word> FPR
%token <y_word> CR
%token <y_word> C

%token <y_word> OP
%token <y_word> OP_BF
%token <y_word> OP_BF_BFA
%token <y_word> OP_BF_FRA_FRB
%token <y_word> OP_BF_L_RA_RB
%token <y_word> OP_BF_L_RA_SI
%token <y_word> OP_BF_L_RA_UI
%token <y_word> OP_BF_U_C
%token <y_word> OP_BO_BI_BDA
%token <y_word> OP_BO_BI_BDL
%token <y_word> OP_BO_BI_BH
%token <y_word> OP_BT_C
%token <y_word> OP_BT_BA_BB
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
%token <y_word> OP_L_RB
%token <y_word> OP_RA_RB
%token <y_word> OP_RB
%token <y_word> OP_RS
%token <y_word> OP_RS_FXM
%token <y_word> OP_RS_L
%token <y_word> OP_RS_RA
%token <y_word> OP_RS_RA_C
%token <y_word> OP_RS_RA_D
%token <y_word> OP_RS_RA_DS
%token <y_word> OP_RS_RA_NB
%token <y_word> OP_RS_RA_RB
%token <y_word> OP_RS_RA_RB_C
%token <y_word> OP_RS_RA_RB_MB5_ME5_C
%token <y_word> OP_RS_RA_RB_MB6_C
%token <y_word> OP_RS_RA_RB_ME6_C
%token <y_word> OP_RS_RA_SH_MB5_ME5_C
%token <y_word> OP_RS_RA_SH_MB6_SH_C
%token <y_word> OP_RS_RA_SH_ME6_SH_C
%token <y_word> OP_RS_RA_SH5_C
%token <y_word> OP_RS_RA_SH6_C
%token <y_word> OP_RS_RA_UI
%token <y_word> OP_RS_RA_UI_CC
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
%token <y_word> OP_RT_RB
%token <y_word> OP_RT_SPR
%token <y_word> OP_RT_SR
%token <y_word> OP_RT_TBR
%token <y_word> OP_TH_RA_RB
%token <y_word> OP_TO_RA_RB
%token <y_word> OP_TO_RA_SI

%token <y_word> OP_la

/* Other token types */

%type <y_word> c
%type <y_word> e16 u8 u7 u6 u5 u4 u2 u1
%type <y_word> nb ds bda bdl lia lil
