/*
 * Sources of the "MISCELLANEOUS" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"trap.h"
#include	"warn.h"
#include	"mem.h"
#include	"memdirect.h"
#include	"shadow.h"
#include	"text.h"
#include	"read.h"
#include	"fra.h"
#include	"rsb.h"
#include	"linfil.h"

extern int running;			/* from main.c */

/* Two useful but unofficial registers */
long LIN;
ptr FIL;

PRIVATE index_jump(), range_check(), search_jump();
PRIVATE gto();

#define asp(l)		newSP(SP + arg_f(l))

DoASPl2(arg)
	long arg;
{
	/* ASP f: Adjust the stack pointer by f */
	register long l = (L_arg_2() * arg);

	LOG(("@M6 DoASPl2(%ld)", l));
	asp(l);
}

DoASPl4(arg)
	long arg;
{
	/* ASP f: Adjust the stack pointer by f */
	register long l = (L_arg_4() * arg);

	LOG(("@M6 DoASPl4(%ld)", l));
	asp(l);
}

DoASPm(arg)
	long arg;
{
	/* ASP f: Adjust the stack pointer by f */
	register long l = arg;

	LOG(("@M6 DoASPm(%ld)", l));
	asp(l);
}

DoASPs(hob, wfac)
	long hob;
	size wfac;
{
	/* ASP f: Adjust the stack pointer by f */
	register long l = (S_arg(hob) * wfac);

	LOG(("@M6 DoASPs(%ld)", l));
	asp(l);
}

DoASSl2(arg)
	size arg;
{
	/* ASS w: Adjust the stack pointer by w-byte integer */
	register size l = (L_arg_2() * arg);

	LOG(("@M6 DoASSl2(%ld)", l));
	spoilFRA();
	l = spop(arg_wi(l));
	asp(l);
}

DoASSz()
{
	/* ASS w: Adjust the stack pointer by w-byte integer */
	register size l = upop(wsize);

	LOG(("@M6 DoASSz(%ld)", l));
	spoilFRA();
	l = spop(arg_wi(l));
	asp(l);
}

#define	block_move(a1,a2,n)	\
		if (in_stack(a1)) { \
			if (in_stack(a2)) st_mvs(a1, a2, n); \
			else st_mvd(a1, a2, n); } \
		else {	if (in_stack(a2)) dt_mvs(a1, a2, n); \
			else dt_mvd(a1, a2, n); }

DoBLMl2(arg)
	size arg;
{
	/* BLM z: Block move z bytes; first pop destination addr, then source addr */
	register size l = (L_arg_2() * arg);
	register ptr dp1, dp2;		/* Destination Pointers */

	LOG(("@M6 DoBLMl2(%ld)", l));
	spoilFRA();
	dp1 = dppop();
	dp2 = dppop();
	block_move(dp1, dp2, arg_z(l));
}

DoBLMl4(arg)
	size arg;
{
	/* BLM z: Block move z bytes; first pop destination addr, then source addr */
	register size l = (L_arg_4() * arg);
	register ptr dp1, dp2;		/* Destination Pointer */

	LOG(("@M6 DoBLMl4(%ld)", l));
	spoilFRA();
	dp1 = dppop();
	dp2 = dppop();
	block_move(dp1, dp2, arg_z(l));
}

DoBLMs(hob, wfac)
	long hob;
	size wfac;
{
	/* BLM z: Block move z bytes; first pop destination addr, then source addr */
	register size l = (S_arg(hob) * wfac);
	register ptr dp1, dp2;		/* Destination Pointer */

	LOG(("@M6 DoBLMs(%ld)", l));
	spoilFRA();
	dp1 = dppop();
	dp2 = dppop();
	block_move(dp1, dp2, arg_z(l));
}

DoBLSl2(arg)
	size arg;
{
	/* BLS w: Block move, size is in w-byte integer on top of stack */
	register size l = (L_arg_2() * arg);
	register ptr dp1, dp2;

	LOG(("@M6 DoBLSl2(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	dp1 = dppop();
	dp2 = dppop();
	block_move(dp1, dp2, arg_z(l));
}

DoBLSz()
{
	/* BLS w: Block move, size is in w-byte integer on top of stack */
	register size l = upop(wsize);
	register ptr dp1, dp2;

	LOG(("@M6 DoBLSz(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	dp1 = dppop();
	dp2 = dppop();
	block_move(dp1, dp2, arg_z(l));
}

DoCSAl2(arg)
	size arg;
{
	/* CSA w: Case jump; address of jump table at top of stack */
	register size l = (L_arg_2() * arg);

	LOG(("@M6 DoCSAl2(%ld)", l));
	spoilFRA();
	index_jump(arg_wi(l));
}

DoCSAm(arg)
	size arg;
{
	/* CSA w: Case jump; address of jump table at top of stack */
	LOG(("@M6 DoCSAm(%ld)", arg));
	spoilFRA();
	index_jump(arg_wi(arg));
}

DoCSAz()
{
	/* CSA w: Case jump; address of jump table at top of stack */
	register size l = upop(wsize);

	LOG(("@M6 DoCSAz(%ld)", l));
	spoilFRA();
	index_jump(arg_wi(l));
}

DoCSBl2(arg)
	size arg;
{
	/* CSB w: Table lookup jump; address of jump table at top of stack */
	register size l = (L_arg_2() * arg);

	LOG(("@M6 DoCSBl2(%ld)", l));
	spoilFRA();
	search_jump(arg_wi(l));
}

DoCSBm(arg)
	size arg;
{
	/* CSB w: Table lookup jump; address of jump table at top of stack */
	LOG(("@M6 DoCSBm(%ld)", arg));
	spoilFRA();
	search_jump(arg_wi(arg));
}

DoCSBz()
{
	/* CSB w: Table lookup jump; address of jump table at top of stack */
	register size l = upop(wsize);

	LOG(("@M6 DoCSBz(%ld)", l));
	spoilFRA();
	search_jump(arg_wi(l));
}

DoDCHz()
{
	/* DCH -: Follow dynamic chain, convert LB to LB of caller */
	register ptr lb;

	LOG(("@M6 DoDCHz()"));
	spoilFRA();
	lb = dppop();
	if (!is_LB(lb)) {
		wtrap(WDCHBADLB, ESTACK);
	}
	dppush(st_lddp(lb + rsb_LB));
}

DoDUPl2(arg)
	size arg;
{
	/* DUP s: Duplicate top s bytes */
	register size l = (L_arg_2() * arg);
	register ptr oldSP = SP;

	LOG(("@M6 DoDUPl2(%ld)", l));
	spoilFRA();
	st_inc(arg_s(l));
	st_mvs(SP, oldSP, l);
}

DoDUPm(arg)
	size arg;
{
	/* DUP s: Duplicate top s bytes */
	register ptr oldSP = SP;

	LOG(("@M6 DoDUPm(%ld)", arg));
	spoilFRA();
	st_inc(arg_s(arg));
	st_mvs(SP, oldSP, arg);
}

DoDUSl2(arg)
	size arg;
{
	/* DUS w: Duplicate top w bytes */
	register size l = (L_arg_2() * arg);
	register ptr oldSP;

	LOG(("@M6 DoDUSl2(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	oldSP = SP;
	st_inc(arg_s(l));
	st_mvs(SP, oldSP, l);
}

DoDUSz()
{
	/* DUS w: Duplicate top w bytes */
	register size l = upop(wsize);
	register ptr oldSP;

	LOG(("@M6 DoDUSz(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	oldSP = SP;
	st_inc(arg_s(l));
	st_mvs(SP, oldSP, l);
}

DoEXGl2(arg)
	size arg;
{
	/* EXG w: Exchange top w bytes */
	register size l = (L_arg_2() * arg);
	register ptr oldSP = SP;

	LOG(("@M6 DoEXGl2(%ld)", l));
	spoilFRA();
	st_inc(arg_w(l));
	st_mvs(SP, oldSP, l);
	st_mvs(oldSP, oldSP + l, l);
	st_mvs(oldSP + l, SP, l);
	st_dec(l);
}

DoEXGs(hob, wfac)
	long hob;
	size wfac;
{
	/* EXG w: Exchange top w bytes */
	register size l = (S_arg(hob) * wfac);
	register ptr oldSP = SP;

	LOG(("@M6 DoEXGs(%ld)", l));
	spoilFRA();
	st_inc(arg_w(l));
	st_mvs(SP, oldSP, l);
	st_mvs(oldSP, oldSP + l, l);
	st_mvs(oldSP + l, SP, l);
	st_dec(l);
}

DoEXGz()
{
	/* EXG w: Exchange top w bytes */
	register size l = upop(wsize);
	register ptr oldSP = SP;

	LOG(("@M6 DoEXGz(%ld)", l));
	spoilFRA();
	st_inc(arg_w(l));
	st_mvs(SP, oldSP, l);
	st_mvs(oldSP, oldSP + l, l);
	st_mvs(oldSP + l, SP, l);
	st_dec(l);
}

DoFILu(arg)
	long arg;
{
	/* FIL g: File name (external 4 := g) */
	register ptr p = i2p(U_arg() * arg);

	LOG(("@M6 DoFILu(%lu)", p));
	spoilFRA();
	if (p > HB) {
		wtrap(WILLFIL, EILLINS);
	}
	putFIL(arg_g(p));
}

DoFILl4(arg)
	long arg;
{
	/* FIL g: File name (external 4 := g) */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@M6 DoFILl4(%lu)", p));
	spoilFRA();
	if (p > HB) {
		wtrap(WILLFIL, EILLINS);
	}
	putFIL(arg_g(p));
}

DoGTOu(arg)
	long arg;
{
	/* GTO g: Non-local goto, descriptor at g */
	register ptr p = i2p(U_arg() * arg);

	LOG(("@M6 DoGTOu(%lu)", p));
	gto(arg_gto(p));
}

DoGTOl4(arg)
	long arg;
{
	/* GTO g: Non-local goto, descriptor at g */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@M6 DoGTOl4(%lu)", p));
	gto(arg_gto(p));
}

DoLIMz()
{
	/* LIM -: Load 16 bit ignore mask */
	LOG(("@M6 DoLIMz()"));
	spoilFRA();
	npush(IgnMask, wsize);
}

DoLINl2(arg)
	long arg;
{
	/* LIN n: Line number (external 0 := n) */
	register unsigned long l = (L_arg_2() * arg);

	LOG(("@M6 DoLINl2(%lu)", l));
	spoilFRA();
	putLIN((long) arg_lin(l));
}

DoLINl4(arg)
	long arg;
{
	/* LIN n: Line number (external 0 := n) */
	register unsigned long l = (L_arg_4() * arg);

	LOG(("@M6 DoLINl4(%lu)", l));
	spoilFRA();
	putLIN((long) arg_lin(l));
}

DoLINs(hob, wfac)
	long hob;
	size wfac;
{
	/* LIN n: Line number (external 0 := n) */
	register unsigned long l = (S_arg(hob) * wfac);

	LOG(("@M6 DoLINs(%lu)", l));
	spoilFRA();
	putLIN((long) arg_lin(l));
}

DoLNIz()
{
	/* LNI -: Line number increment */
	LOG(("@M6 DoLNIz()"));
	spoilFRA();
	putLIN((long)getLIN() + 1);
}

DoLORs(hob, wfac)
	long hob;
	size wfac;
{
	/* LOR r: Load register (0=LB, 1=SP, 2=HP) */
	register long l = (S_arg(hob) * wfac);

	LOG(("@M6 DoLORs(%ld)", l));
	spoilFRA();
	switch ((int) arg_r(l)) {
	case 0:
		dppush(LB);
		break;
	case 1:
		dppush(SP);
		break;
	case 2:
		dppush(HP);
		break;
	}
}

DoLPBz()
{
	/* LPB -: Convert local base to argument base */
	register ptr lb;

	LOG(("@M6 DoLPBz()"));
	spoilFRA();
	lb = dppop();
	if (!is_LB(lb)) {
		wtrap(WLPBBADLB, ESTACK);
	}
	dppush(lb + rsbsize);
}

DoMONz()
{
	/* MON -: Monitor call */
	LOG(("@M6 DoMONz()"));
	spoilFRA();
	moncall();
}

DoNOPz()
{
	/* NOP -: No operation */
	LOG(("@M6 DoNOPz()"));
	spoilFRA();
	message("NOP instruction");
}

DoRCKl2(arg)
	size arg;
{
	/* RCK w: Range check; trap on error */
	register size l = (L_arg_2() * arg);

	LOG(("@M6 DoRCKl2(%ld)", l));
	spoilFRA();
	range_check(arg_wi(l));
}

DoRCKm(arg)
	size arg;
{
	/* RCK w: Range check; trap on error */
	LOG(("@M6 DoRCKm(%ld)", arg));
	spoilFRA();
	range_check(arg_wi(arg));
}

DoRCKz()
{
	/* RCK w: Range check; trap on error */
	register size l = upop(wsize);

	LOG(("@M6 DoRCKz(%ld)", l));
	spoilFRA();
	range_check(arg_wi(l));
}

DoRTTz()
{
	/* RTT -: Return from trap */
	LOG(("@M6 DoRTTz()"));

	switch (poprsb(1)) {
	case RSB_STP:
		warning(WRTTEMPTY);
		running = 0;		/* stop the machine */
		return;
	case RSB_CAL:
		warning(WRTTCALL);
		return;
	case RSB_RTT:
		/* OK */
		break;
	case RSB_NRT:
		warning(WRTTNRTT);
		running = 0;		/* stop the machine */
		return;
	default:
		warning(WRTTBAD);
		return;
	}

	/* pop the trap number */
	upop(wsize);
	
	/* restore the Function Return Area */
	FRA_def = upop(wsize);
	FRASize = upop(wsize);
	popFRA(FRASize);
}

DoSIGz()
{
	/* SIG -: Trap errors to proc identifier on top of stack, \-2 resets default */
 	register long tpi = spop(psize);

	LOG(("@M6 DoSIGz()"));
	spoilFRA();
	npush(TrapPI, psize);
	if (tpi == -2) {
		OnTrap = TR_HALT;
		TrapPI = 0;
	}
	else {
		tpi = arg_p(tpi);	/* do not test earlier! */
		OnTrap = TR_TRAP;
		TrapPI = tpi;
	}
}

DoSIMz()
{
	/* SIM -: Store 16 bit ignore mask */
	LOG(("@M6 DoSIMz()"));
	spoilFRA();
	IgnMask = (upop(wsize) | PreIgnMask) & MASK2;
}

DoSTRs(hob, wfac)
	long hob;
	size wfac;
{
	/* STR r: Store register (0=LB, 1=SP, 2=HP) */
	register long l = (S_arg(hob) * wfac);

	LOG(("@M6 DoSTRs(%ld)", l));
	spoilFRA();
	switch ((int) arg_r(l)) {
	case 0:
		newLB(dppop());
		pop_frames();
		break;
	case 1:
		newSP(dppop());
		break;
	case 2:
		newHP(dppop());
		break;
	}
}

DoTRPz()
{
	/* TRP -: Cause trap to occur (Error number on stack) */
	register unsigned int tr = (unsigned int)upop(wsize);

	LOG(("@M6 DoTRPz()"));
	spoilFRA();
	if (tr > 15 || !(IgnMask&BIT(tr))) {
		wtrap(WTRP, (int)tr);
	}
}


/* Service routines */

PRIVATE gto(p)
	ptr p;
{
	register ptr old_LB = LB;
	register ptr new_PC = dt_ldip(p);
	register ptr new_SP = dt_lddp(p + psize);
	register ptr new_LB = dt_lddp(p + (2 * psize));

	while (old_LB < new_LB) {
		PI = st_lds(old_LB + rsb_PI, psize);
		old_LB = st_lddp(old_LB + rsb_LB);
	}
	if (old_LB != new_LB) {
		wtrap(WGTORSB, EBADGTO);
	}

	newLB(new_LB);
	pop_frames();
	newSP(new_SP);
	newPC(new_PC);
}

/*
	The LIN and FIL routines.
	The values of LIN and FIL are kept in EM machine registers
	(variables LIN and FIL) and in the data space.
*/

putLIN(lin)
	long lin;
{
	dt_unprot(i2p(LINO_AD), (long)LINSIZE);
	dt_stn(i2p(LINO_AD), lin, (long)LINSIZE);
	LIN = lin;
	dt_prot(i2p(LINO_AD), (long)LINSIZE);
}

putFIL(fil)
	ptr fil;
{
	dt_unprot(i2p(FILN_AD), psize);
	dt_stdp(i2p(FILN_AD), fil);
	FIL = fil;
	dt_prot(i2p(FILN_AD), psize);
}

/********************************************************
 *		Case jump by indexing			*
 *							*
 *	1. pop case descriptor pointer.			*
 *	2. pop table index.				*
 *	3. Calculate (table index) - (lower bound).	*
 *	4. Check if in range.				*
 *	5. If in range: load Program Counter value.	*
 *	6. Else: load default value.			*
 ********************************************************/

PRIVATE index_jump(nbytes)
	size nbytes;
{
	register ptr cdp = dppop();	/* Case Descriptor Pointer */
	register long t_index =		/* Table INDEX */
			spop(nbytes) - mem_lds(cdp + psize, wsize);
	register ptr nPC;		/* New Program Counter */

	if (t_index >= 0 && t_index <= mem_lds(cdp + wsize + psize, wsize)) {
		nPC = mem_ldip(cdp + (2 * wsize) + ((t_index + 1) * psize));
	}
	else if ((nPC = mem_ldip(cdp)) == 0) {
		trap(ECASE);
	}
	newPC(nPC);
}

/********************************************************
 *		Case jump by table search		*
 *							*
 *	1. pop case descriptor pointer.			*
 *	2. pop search value.				*
 *	3. Load number of table entries.		*
 *	4. Check if search value in table.		*
 *	5. If found: load Program Counter value.	*
 *	6. Else: load default value.			*
 ********************************************************/

PRIVATE search_jump(nbytes)
	size nbytes;
{
	register ptr cdp = dppop();	/* Case Descriptor Pointer */
	register long sv = spop(nbytes);/* Search Value */
	register long nt =		/* Number of Table-entries */
			mem_lds(cdp + psize, wsize);
	register ptr nPC;		/* New Program Counter */

	while (--nt >= 0) {
		if (sv == mem_lds(cdp + (nt+1) * (wsize+psize), wsize)) {
			nPC = mem_ldip(cdp + wsize + (nt+1)*(wsize+psize));
			if (nPC == 0)
				trap(ECASE);
			newPC(nPC);
			return;
		}
	}
	nPC = mem_ldip(cdp);
	if (nPC == 0)
		trap(ECASE);
	newPC(nPC);
}

/********************************************************
 *			Range check			*
 *							*
 *	1. Load range descriptor.			*
 *	2. Check against lower and upper bound.		*
 *	3. Generate trap if necessary.			*
 *	4. DON'T remove integer.			*
 ********************************************************/

PRIVATE range_check(nbytes)
	size nbytes;
{
	register ptr rdp = dppop();	/* Range check Descriptor Pointer */
	register long cv =		/* Check Value */
			st_lds(SP, nbytes);

	if (must_test && !(IgnMask&BIT(ERANGE))) {
		if (	cv < mem_lds(rdp, wsize)
		||	cv > mem_lds(rdp + wsize, wsize)
		) {
			trap(ERANGE);
		}
	}
}
