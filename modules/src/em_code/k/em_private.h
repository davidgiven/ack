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
#define	DFILB(x)	put_ilb(x)
#define	NOFF(x,y)	put_noff((x), (y))
#define	DOFF(x,y)	put_doff((x), (y))
#define	PNAM(x)		put_pnam(x)
#define	DNAM(x)		put_dnam(x)
#define	DFDNAM(x)	put_dnam(x)
#define	CEND()		put_cend()
#define CCEND()		put_cend()
#define	WCON(x,y,z)	put_wcon((x), (y), (z))
#define COMMA()
#define NL()
#define CILB(x)		CST(x)

#define	put_cend()	putbyte(sp_cend)
#define	put_op(x)	putbyte(x)
#define	put_ps(x)	putbyte(x)
