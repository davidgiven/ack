/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* private inclusion file */

#include <em_arith.h>
#include <em_label.h>
#include <em_code.h>

/* include the EM description files */
#include	<em_spec.h>
#include	<em_pseu.h>
#include	<em_mnem.h>
#include	<em_reg.h>

/* macros used in the definitions of the interface functions C_* */
#define	OP(x)		put_op(x)
#define	CST(x)		put_cst(x)
#define	DCST(x)		put_cst(x)
#define	SCON(x,y)	put_scon((x), (y))
#define	PS(x)		put_ps(x)
#define	DLB(x)		put_dlb(x)
#define DFDLB(x)	put_dlb(x)
#define	ILB(x)		put_ilb(x)
#define	DFILB(x)	put_dfilb(x)
#define	NOFF(x,y)	put_noff((x), (y))
#define	DOFF(x,y)	put_doff((x), (y))
#define	PNAM(x)		put_pnam(x)
#define	DNAM(x)		_prnt(x)
#define	DFDNAM(x)	_prnt(x)
#define	CEND()
#define CCEND()		put_ccend()
#define	WCON(x,y,z)	put_wcon((x), (y), (z))
#define COMMA()		put_comma()
#define NL()		put_nl()
#define CILB(x)		put_ilb(x)
