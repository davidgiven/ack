/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define	ptyp(x)		(1<<(x-sp_fspec))

#define	cst_ptyp	(ptyp(sp_cst2)|ptyp(sp_cst4))
#define nof_ptyp	(ptyp(sp_dlb1)|ptyp(sp_dlb2)|ptyp(sp_doff))
#define sof_ptyp	(ptyp(sp_dnam)|ptyp(sp_doff))
#define lab_ptyp	(ptyp(sp_dlb1)|ptyp(sp_dlb2)|ptyp(sp_dnam))
#define ico_ptyp	(ptyp(sp_icon))
#define uco_ptyp	(ptyp(sp_ucon))
#define fco_ptyp	(ptyp(sp_fcon))
#define str_ptyp	(ptyp(sp_scon))
#define	con_ptyp	(str_ptyp|ico_ptyp|uco_ptyp|fco_ptyp)
#define ilb_ptyp	(ptyp(sp_ilb1)|ptyp(sp_ilb2))
#define pro_ptyp	(ptyp(sp_pnam))
#define off_ptyp	(ptyp(sp_doff))
#define end_ptyp	(ptyp(sp_cend))
#define	sym_ptyp	(lab_ptyp)
#define	arg_ptyp	(nof_ptyp|cst_ptyp|sof_ptyp)
#define	par_ptyp	(arg_ptyp|ico_ptyp|uco_ptyp|fco_ptyp|pro_ptyp|ilb_ptyp)
#define val_ptyp	(par_ptyp|str_ptyp)
#define	any_ptyp	(val_ptyp|end_ptyp)
