/*
 * Sources of the "PROCEDURE CALL" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"shadow.h"
#include	"memdirect.h"
#include	"trap.h"
#include	"warn.h"
#include	"text.h"
#include	"proctab.h"
#include	"fra.h"
#include	"rsb.h"
#include	"linfil.h"

extern int running;			/* from main.c */

PRIVATE lfr(), ret();

DoCAIz()				/* proc identifier on top of stack */
{
	/* CAI -: Call procedure (procedure identifier on stack) */
	register long pi = spop(psize);

	LOG(("@P6 DoCAIz(%lu)", pi));
	call(arg_p(pi), RSB_CAL);
}

DoCALl2(arg)
	long arg;
{
	/* CAL p: Call procedure (with identifier p) */
	register long pi = (L_arg_2() * arg);

	LOG(("@P6 DoCALl2(%lu)", pi));
	call(arg_p(pi), RSB_CAL);
}

DoCALl4(arg)
	long arg;
{
	/* CAL p: Call procedure (with identifier p) */
	register long pi = (L_arg_4() * arg);

	LOG(("@P6 DoCALl4(%lu)", pi));
	call(arg_p(pi), RSB_CAL);
}

DoCALm(arg)
	long arg;
{
	/* CAL p: Call procedure (with identifier p) */
	register long pi = arg_p(arg);

	LOG(("@P6 DoCALm(%lu)", pi));
	call(pi, RSB_CAL);
}

DoCALs(hob, wfac)
	long hob;
	size wfac;
{
	/* CAL p: Call procedure (with identifier p) */
	register long pi = (S_arg(hob) * wfac);

	LOG(("@P6 DoCALs(%lu)", pi));
	call(arg_p(pi), RSB_CAL);
}

DoLFRl2(arg)
	size arg;
{
	/* LFR s: Load function result */
	register size l = (L_arg_2() * arg);

	LOG(("@P6 DoLFRl2(%ld)", l));
	lfr(arg_s(l));
}

DoLFRm(arg)
	size arg;
{
	/* LFR s: Load function result */
	LOG(("@P6 DoLFRm(%ld)", arg));
	lfr(arg_s(arg));
}

DoLFRs(hob, wfac)
	long hob;
	size wfac;
{
	/* LFR s: Load function result */
	register size l = (S_arg(hob) * wfac);

	LOG(("@P6 DoLFRs(%ld)", l));
	lfr(arg_s(l));
}

DoRETl2(arg)
	size arg;
{
	/* RET z: Return (function result consists of top z bytes) */
	register size l = (L_arg_2() * arg);

	LOG(("@P6 DoRETl2(%ld)", l));
	ret(arg_z(l));
}

DoRETm(arg)
	size arg;
{
	/* RET z: Return (function result consists of top z bytes) */
	LOG(("@P6 DoRETm(%ld)", arg));
	ret(arg_z(arg));
}

DoRETs(hob, wfac)
	long hob;
	size wfac;
{
	/* RET z: Return (function result consists of top z bytes) */
	register size l = (S_arg(hob) * wfac);

	LOG(("@P6 DoRETs(%ld)", l));
	ret(arg_z(l));
}

/************************************************************************
 *		Calling a new procedure.				*
 ************************************************************************/

call(new_PI, rsbcode)
	long new_PI;
	int rsbcode;
{
	/* legality of new_PI has already been checked */
	register size nloc = proctab[new_PI].pr_nloc;
	register ptr ep = proctab[new_PI].pr_ep;

	push_frame(SP);			/* remember AB */
	pushrsb(rsbcode);

	/* do the call */
	PI = new_PI;
	st_inc(nloc);
	newPC(ep);
	spoilFRA();
	LOG(("@p5 call: new_PI = %lu, nloc = %lu, ep = %lu",
				new_PI, nloc, ep));
}

/************************************************************************
 *		Loading a function result.				*
 ************************************************************************/

PRIVATE lfr(sz)
	size sz;
{
	if (sz > FRALimit) {
		wtrap(WILLLFR, EILLINS);
	}

	LOG(("@p5 lfr: size = %ld", sz));

#ifdef	LOGGING
	if (!FRA_def) {
		warning(WRFUNGAR);
	}
	if (sz != FRASize) {
		warning(FRASize < sz ? WRFUNSML : WRFUNLAR);
	}
#endif	LOGGING

	pushFRA(sz);
	spoilFRA();
}

/************************************************************************
 *		Returning from a procedure.				*
 ************************************************************************/

PRIVATE ret(sz)
	size sz;
{
	if (sz > FRALimit) {
		wtrap(WILLRET, EILLINS);
	}

	LOG(("@p5 ret: size = %ld", sz));

	/* retrieve return value from stack */
	FRA_def = DEFINED;
	FRASize = sz;
	popFRA(FRASize);

	switch (poprsb(0)) {
	case RSB_STP:
		if (sz == wsize) {
			ES_def = DEFINED;
			ES = btol(FRA[sz-1]);
					/* one byte only */
		}
		running = 0;		/* stop the machine */
		return;
	case RSB_CAL:
		/* OK */
		break;
	case RSB_RTT:
	case RSB_NRT:
		warning(WRETTRAP);
		running = 0;		/* stop the machine */
		return;
	default:
		warning(WRETBAD);
		return;
	}
}

