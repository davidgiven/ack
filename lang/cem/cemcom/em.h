/* $Header$ */
/* DESCRIPTION OF INTERFACE TO EM CODE GENERATING ROUTINES */

#include "proc_intf.h"	/* use macros or functions */

/* include the EM description files */
#include	<em_spec.h>
#include	<em_pseu.h>
#include	<em_mes.h>
#include	<em_mnem.h>
#include	<em_reg.h>

/* macros used in the definitions of the interface functions C_* */
#define	OP(x)		put_op(x)
#define	CST(x)		put_cst(x)
#define	DCST(x)		put_cst(x)
#define	CSTR(x)		put_cstr(x)
#define	PS(x)		put_ps(x)
#define	DLB(x)		put_dlb(x)
#define	ILB(x)		put_ilb(x)
#define	NOFF(x,y)	put_noff((x), (y))
#define	DOFF(x,y)	put_doff((x), (y))
#define	PNAM(x)		put_pnam(x)
#define	DNAM(x)		put_dnam(x)
#define	CEND()		put_cend()
#define	WCON(x,y,z)	put_wcon((x), (y), (z))
#define	FCON(x,y)	put_fcon((x), (y))

/* variants of primitive "putbyte" */
#define	put8(x)		putbyte(x)	/* defined in "em.c" */
#define	put16(x)	(put8((int) x), put8((int) (x >> 8)))
#define	put32(x)	(put16((int) x), put16((int) (x >> 16)))
#define	put_cend()	put8(sp_cend)
#define	put_op(x)	put8(x)
#define	put_ps(x)	put8(x)

/* user interface */
#define C_magic()	put16(sp_magic)	/* EM magic word */

#ifndef PROC_INTF
#include "writeem.h"
#endif PROC_INTF
