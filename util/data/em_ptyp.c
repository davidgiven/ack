#include	<em_spec.h>
#include	<em_ptyp.h>

short	em_ptyp[] = {
	0,			/* PAR_NO */
	cst_ptyp,		/* PAR_C */
	cst_ptyp,		/* PAR_D */
	cst_ptyp,		/* PAR_N */
	cst_ptyp,		/* PAR_F */
	cst_ptyp,		/* PAR_L */
	arg_ptyp,		/* PAR_G */
	cst_ptyp|ptyp(sp_cend),	/* PAR_W */
	cst_ptyp,		/* PAR_S */
	cst_ptyp,		/* PAR_Z */
	cst_ptyp,		/* PAR_O */
	ptyp(sp_pnam),		/* PAR_P */
	ptyp(sp_cst2),		/* PAR_B */
	ptyp(sp_cst2)		/* PAR_R */
};
