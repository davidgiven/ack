/*
 * Sources of the "COMPARE" group instructions
 */

/* $Id$ */

#include	<em_abs.h>
#include	"logging.h"
#include	"nofloat.h"
#include	"global.h"
#include	"log.h"
#include	"warn.h"
#include	"mem.h"
#include	"shadow.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"

#ifndef	NOFLOAT
extern double fpop();
#endif	/* NOFLOAT */

PRIVATE compare_obj();

DoCMI(l)
	register size l;
{
	/* CMI w: Compare w byte integers, Push negative, zero, positive for <, = or > */
	register long t = spop(arg_wi(l));
	register long s = spop(l);

	LOG(("@T6 DoCMI(%ld)", l));
	spoilFRA();
	wpush((long)(t < s ? 1 : t > s ? -1 : 0));
}

DoCMF(l)
	register size l;
{
	/* CMF w: Compare w byte reals */
#ifndef	NOFLOAT
	double t = fpop(arg_wf(l));
	double s = fpop(l);

	LOG(("@T6 DoCMF(%ld)", l));
	spoilFRA();
	wpush((long)(t < s ? 1 : t > s ? -1 : 0));
#else	/* NOFLOAT */
	nofloat();
#endif	/* NOFLOAT */
}

DoCMU(l)
	register size l;
{
	/* CMU w: Compare w byte unsigneds */
	register unsigned long t = upop(arg_wi(l));
	register unsigned long s = upop(l);

	LOG(("@T6 DoCMU(%ld)", l));
	spoilFRA();
	wpush((long)(t < s ? 1 : t > s ? -1 : 0));
}

DoCMS(l)
	register size l;
{
	/* CMS w: Compare w byte values, can only be used for bit for bit equality test */

	LOG(("@T6 DoCMS(%ld)", l));
	spoilFRA();
	compare_obj(arg_w(l));
}

DoCMP()
{
	/* CMP -: Compare pointers */
	register ptr t, s;

	LOG(("@T6 DoCMP()"));
	spoilFRA();
	t = dppop();
	s = dppop();
	wpush((long)(t < s ? 1 : t > s ? -1 : 0));
}

DoTLT()
{
	/* TLT -: True if less, i.e. iff top of stack < 0 */
	LOG(("@T6 DoTLT()"));
	spoilFRA();
	wpush((long)(wpop() < 0 ? 1 : 0));
}

DoTLE()
{
	/* TLE -: True if less or equal, i.e. iff top of stack <= 0 */
	LOG(("@T6 DoTLE()"));
	spoilFRA();
	wpush((long)(wpop() <= 0 ? 1 : 0));
}

DoTEQ()
{
	/* TEQ -: True if equal, i.e. iff top of stack = 0 */
	LOG(("@T6 DoTEQ()"));
	spoilFRA();
	wpush((long)(wpop() == 0 ? 1 : 0));
}

DoTNE()
{
	/* TNE -: True if not equal, i.e. iff top of stack non zero */
	LOG(("@T6 DoTNE()"));
	spoilFRA();
	wpush((long)(wpop() != 0 ? 1 : 0));
}

DoTGE()
{
	/* TGE -: True if greater or equal, i.e. iff top of stack >= 0 */
	LOG(("@T6 DoTGE()"));
	spoilFRA();
	wpush((long)(wpop() >= 0 ? 1 : 0));
}

DoTGT()
{
	/* TGT -: True if greater, i.e. iff top of stack > 0 */
	LOG(("@T6 DoTGT()"));
	spoilFRA();
	wpush((long)(wpop() > 0 ? 1 : 0));
}

/********************************************************
 *		Compare objects				*
 *							*
 *	Two 'obj_size' sized objects are bytewise	*
 *	compared; as soon as one byte is different	*
 *	1 is returned, otherwise 0. No type checking	*
 *	is performed. Checking for undefined bytes	*
 *	is done when LOGGING is defined.		*
 ********************************************************/

PRIVATE compare_obj(obj_size)
	size obj_size;
{
	register ptr addr1;		/* ADDRess in object highest on st. */
	register ptr addr2;		/* ADDRess in object deeper in st. */
	register int comp_res = 0;	/* COMPare RESult */

	for (	addr1 = SP, addr2 = SP + obj_size;
		addr1 < SP + obj_size;
		addr1++, addr2++
	) {
#ifdef	LOGGING
		if (!st_sh(addr1) || !st_sh(addr2)) {
			warning(WUNCMP);
			/* Let's say undefined's are not equal: */
			comp_res = 1;
			break;
		}
#endif	/* LOGGING */
		if (stack_loc(addr1) != stack_loc(addr2)) {
			comp_res = 1;
			break;
		}
	}
	st_dec(2 * obj_size);
	wpush((long) comp_res);
}
