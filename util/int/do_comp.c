/*
 * Sources of the "COMPARE" group instructions
 */

/* $Header$ */

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
#endif	NOFLOAT

PRIVATE compare_obj();

DoCMIl2(arg)
	size arg;
{
	/* CMI w: Compare w byte integers, Push negative, zero, positive for <, = or > */
	register size l = (L_arg_2() * arg);
	register long t = spop(arg_wi(l));
	register long s = spop(l);

	LOG(("@T6 DoCMIl2(%ld)", l));
	spoilFRA();
	npush((long)(t < s ? 1 : t > s ? -1 : 0), wsize);
}

DoCMIm(arg)
	size arg;
{
	/* CMI w: Compare w byte integers, Push negative, zero, positive for <, = or > */
	register size l = arg_wi(arg);
	register long t = spop(l);
	register long s = spop(l);

	LOG(("@T6 DoCMIm(%ld)", l));
	spoilFRA();
	npush((long)(t < s ? 1 : t > s ? -1 : 0), wsize);
}

DoCMIz()
{
	/* CMI w: Compare w byte integers, Push negative, zero, positive for <, = or > */
	register size l = upop(wsize);
	register long t = spop(arg_wi(l));
	register long s = spop(l);

	LOG(("@T6 DoCMIz(%ld)", l));
	spoilFRA();
	npush((long)(t < s ? 1 : t > s ? -1 : 0), wsize);
}

DoCMFl2(arg)
	size arg;
{
	/* CMF w: Compare w byte reals */
#ifndef	NOFLOAT
	register size l = (L_arg_2() * arg);
	double t = fpop(arg_wf(l));
	double s = fpop(l);

	LOG(("@T6 DoCMFl2(%ld)", l));
	spoilFRA();
	npush((long)(t < s ? 1 : t > s ? -1 : 0), wsize);
#else	NOFLOAT
	arg = arg;
	nofloat();
#endif	NOFLOAT
}

DoCMFs(hob, wfac)
	long hob;
	size wfac;
{
	/* CMF w: Compare w byte reals */
#ifndef	NOFLOAT
	register size l = (S_arg(hob) * wfac);
	double t = fpop(arg_wf(l));
	double s = fpop(l);

	LOG(("@T6 DoCMFs(%ld)", l));
	spoilFRA();
	npush((long)(t < s ? 1 : t > s ? -1 : 0), wsize);
#else	NOFLOAT
	hob = hob;
	wfac = wfac;
	nofloat();
#endif	NOFLOAT
}

DoCMFz()
{
	/* CMF w: Compare w byte reals */
#ifndef	NOFLOAT
	register size l = upop(wsize);
	double t = fpop(arg_wf(l));
	double s = fpop(l);

	LOG(("@T6 DoCMFz(%ld)", l));
	spoilFRA();
	npush((long)(t < s ? 1 : t > s ? -1 : 0), wsize);
#else	NOFLOAT
	nofloat();
#endif	NOFLOAT
}

DoCMUl2(arg)
	size arg;
{
	/* CMU w: Compare w byte unsigneds */
	register size l = (L_arg_2() * arg);
	register unsigned long t = upop(arg_wi(l));
	register unsigned long s = upop(l);

	LOG(("@T6 DoCMUl2(%ld)", l));
	spoilFRA();
	npush((long)(t < s ? 1 : t > s ? -1 : 0), wsize);
}

DoCMUz()
{
	/* CMU w: Compare w byte unsigneds */
	register size l = upop(wsize);
	register unsigned long t = upop(arg_wi(l));
	register unsigned long s = upop(l);

	LOG(("@T6 DoCMUz(%ld)", l));
	spoilFRA();
	npush((long)(t < s ? 1 : t > s ? -1 : 0), wsize);
}

DoCMSl2(arg)
	size arg;
{
	/* CMS w: Compare w byte values, can only be used for bit for bit equality test */
	register size l = (L_arg_2() * arg);

	LOG(("@T6 DoCMSl2(%ld)", l));
	spoilFRA();
	compare_obj(arg_w(l));
}

DoCMSs(hob, wfac)
	long hob;
	size wfac;
{
	/* CMS w: Compare w byte values, can only be used for bit for bit equality test */
	register size l = (S_arg(hob) * wfac);

	LOG(("@T6 DoCMSs(%ld)", l));
	spoilFRA();
	compare_obj(arg_w(l));
}

DoCMSz()
{
	/* CMS w: Compare w byte values, can only be used for bit for bit equality test */
	register size l = upop(wsize);

	LOG(("@T6 DoCMSz(%ld)", l));
	spoilFRA();
	compare_obj(arg_w(l));
}

DoCMPz()
{
	/* CMP -: Compare pointers */
	register ptr t, s;

	LOG(("@T6 DoCMPz()"));
	spoilFRA();
	t = dppop();
	s = dppop();
	npush((long)(t < s ? 1 : t > s ? -1 : 0), wsize);
}

DoTLTz()
{
	/* TLT -: True if less, i.e. iff top of stack < 0 */
	LOG(("@T6 DoTLTz()"));
	spoilFRA();
	npush((long)(wpop() < 0 ? 1 : 0), wsize);
}

DoTLEz()
{
	/* TLE -: True if less or equal, i.e. iff top of stack <= 0 */
	LOG(("@T6 DoTLEz()"));
	spoilFRA();
	npush((long)(wpop() <= 0 ? 1 : 0), wsize);
}

DoTEQz()
{
	/* TEQ -: True if equal, i.e. iff top of stack = 0 */
	LOG(("@T6 DoTEQz()"));
	spoilFRA();
	npush((long)(wpop() == 0 ? 1 : 0), wsize);
}

DoTNEz()
{
	/* TNE -: True if not equal, i.e. iff top of stack non zero */
	LOG(("@T6 DoTNEz()"));
	spoilFRA();
	npush((long)(wpop() != 0 ? 1 : 0), wsize);
}

DoTGEz()
{
	/* TGE -: True if greater or equal, i.e. iff top of stack >= 0 */
	LOG(("@T6 DoTGEz()"));
	spoilFRA();
	npush((long)(wpop() >= 0 ? 1 : 0), wsize);
}

DoTGTz()
{
	/* TGT -: True if greater, i.e. iff top of stack > 0 */
	LOG(("@T6 DoTGTz()"));
	spoilFRA();
	npush((long)(wpop() > 0 ? 1 : 0), wsize);
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
#endif	LOGGING
		if (stack_loc(addr1) != stack_loc(addr2)) {
			comp_res = 1;
			break;
		}
	}
	st_dec(2 * obj_size);
	npush((long) comp_res, wsize);
}
