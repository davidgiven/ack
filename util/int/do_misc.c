/** @file
 *  Sources of the "MISCELLANEOUS" group instructions
 */

/* $Id$ */

#include	<em_abs.h>
#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"trap.h"
#include	"warn.h"
#include	"mem.h"
#include	"memdirect.h"
#include	"segment.h"
#include	"shadow.h"
#include	"data.h"
#include	"text.h"
#include	"stack.h"
#include	"read.h"
#include	"fra.h"
#include	"rsb.h"
#include	"io.h"
#include	"linfil.h"

extern int running;			/* from main.c */

/** Current line number */
long LIN;
/** Pointer to the filename. */
ptr FIL;

PRIVATE void index_jump(size), range_check(size), search_jump(size);
PRIVATE void gto(ptr);
void putLIN(long);
void putFIL(ptr);

#define asp(l)		newSP(SP + arg_f(l))

extern void moncall(void);

/** ASP f: Adjust the stack pointer by f */
void DoASP(register long l)
{
	LOG(("@M6 DoASP(%ld)", l));
	asp(l);
}

/** ASS w: Adjust the stack pointer by w-byte integer */
void DoASS(register size l)
{
	LOG(("@M6 DoASS(%ld)", l));
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

/** BLM z: Block move z bytes; first pop destination addr, then source addr */
void DoBLM(register size l)
{
	register ptr dp1, dp2;		/* Destination Pointers */

	LOG(("@M6 DoBLM(%ld)", l));
	spoilFRA();
	dp1 = dppop();
	dp2 = dppop();
	block_move(dp1, dp2, arg_z(l));
}

/** BLS w: Block move, size is in w-byte integer on top of stack */
void DoBLS(register size l)
{
	register ptr dp1, dp2;

	LOG(("@M6 DoBLS(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	dp1 = dppop();
	dp2 = dppop();
	block_move(dp1, dp2, arg_z(l));
}

/** CSA w: Case jump; address of jump table at top of stack */
void DoCSA(register size l)
{
	LOG(("@M6 DoCSA(%ld)", l));
	spoilFRA();
	index_jump(arg_wi(l));
}

/** CSB w: Table lookup jump; address of jump table at top of stack */
void DoCSB(register size l)
{
	LOG(("@M6 DoCSB(%ld)", l));
	spoilFRA();
	search_jump(arg_wi(l));
}

/** DCH -: Follow dynamic chain, convert LB to LB of caller */
void DoDCH(void)
{
	register ptr lb;

	LOG(("@M6 DoDCH()"));
	spoilFRA();
	lb = dppop();
	if (!is_LB(lb)) {
		wtrap(WDCHBADLB, ESTACK);
	}
	dppush(st_lddp(lb + rsb_LB));
}

/** DUP s: Duplicate top s bytes */
void DoDUP(size arg)
{
	register ptr oldSP = SP;

	LOG(("@M6 DoDUP(%ld)", arg));
	spoilFRA();
	st_inc(arg_s(arg));
	st_mvs(SP, oldSP, arg);
}

/** DUS w: Duplicate top w bytes */
void DoDUS(register size l)
{
	register ptr oldSP;

	LOG(("@M6 DoDUS(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	oldSP = SP;
	st_inc(arg_s(l));
	st_mvs(SP, oldSP, l);
}

/** EXG w: Exchange top w bytes */
void DoEXG(register size l)
{
	register ptr oldSP = SP;

	LOG(("@M6 DoEXG(%ld)", l));
	spoilFRA();
	st_inc(arg_w(l));
	st_mvs(SP, oldSP, l);
	st_mvs(oldSP, oldSP + l, l);
	st_mvs(oldSP + l, SP, l);
	st_dec(l);
}

/** FIL g: File name (external 4 := g) */
void DoFIL(register unsigned long arg)
{
	register ptr p = i2p(arg);

	LOG(("@M6 DoFIL(%lu)", p));
	spoilFRA();
	if (p > HB) {
		wtrap(WILLFIL, EILLINS);
	}
	putFIL(arg_g(p));
}

/** GTO g: Non-local goto, descriptor at g */
void DoGTO(register unsigned long arg)
{
	register ptr p = i2p(arg);

	LOG(("@M6 DoGTO(%lu)", p));
	gto(arg_gto(p));
}

/** LIM -: Load 16 bit ignore mask */
void DoLIM(void)
{
	LOG(("@M6 DoLIM()"));
	spoilFRA();
	wpush(IgnMask);
}

/** LIN n: Line number (external 0 := n) */
void DoLIN(register unsigned long l)
{
	LOG(("@M6 DoLIN(%lu)", l));
	spoilFRA();
	putLIN((long) arg_lin(l));
}

/** LNI -: Line number increment */
void DoLNI(void)
{
	LOG(("@M6 DoLNI()"));
	spoilFRA();
	putLIN((long)getLIN() + 1);
}

/** LOR r: Load register (0=LB, 1=SP, 2=HP) */
void DoLOR(register long l)
{
	LOG(("@M6 DoLOR(%ld)", l));
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

/** LPB -: Convert local base to argument base */
void DoLPB(void)
{
	register ptr lb;

	LOG(("@M6 DoLPB()"));
	spoilFRA();
	lb = dppop();
	if (!is_LB(lb)) {
		wtrap(WLPBBADLB, ESTACK);
	}
	dppush(lb + rsbsize);
}

/** MON -: Monitor call */
void DoMON(void)
{
	LOG(("@M6 DoMON()"));
	spoilFRA();
	moncall();
}

/** NOP -: No operation */
void DoNOP(void)
{
	LOG(("@M6 DoNOP()"));
	spoilFRA();
	message("NOP instruction");
}

/** RCK w: Range check; trap on error */
void DoRCK(register size l)
{
	LOG(("@M6 DoRCK(%ld)", l));
	spoilFRA();
	range_check(arg_wi(l));
}

/** RTT -: Return from trap */
void DoRTT(void)
{
	LOG(("@M6 DoRTT()"));

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
	uwpop();
	
	/* restore the Function Return Area */
	FRA_def = uwpop();
	FRASize = uwpop();
	popFRA(FRASize);
}

/** SIG -: Trap errors to proc identifier on top of stack, \-2 resets default */
void DoSIG(void)
{
 	register long tpi = spop(psize);

	LOG(("@M6 DoSIG()"));
	spoilFRA();
	if (OnTrap == TR_HALT) {
		npush(-2L, psize);
	}
	else	npush(TrapPI, psize);
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

/** SIM -: Store 16 bit ignore mask */
void DoSIM(void)
{
	LOG(("@M6 DoSIM()"));
	spoilFRA();
	IgnMask = (uwpop() | PreIgnMask) & MASK2;
}

/** STR r: Store register (0=LB, 1=SP, 2=HP) */
void DoSTR(register long l)
{
	LOG(("@M6 DoSTR(%ld)", l));
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

/** TRP -: Cause trap to occur (Error number on stack) */
void DoTRP(void)
{
	register unsigned int tr = (unsigned int)uwpop();

	LOG(("@M6 DoTRP()"));
	spoilFRA();
	if (tr > 15 || !(IgnMask&BIT(tr))) {
		wtrap(WTRP, (int)tr);
	}
}


/* Service routines */

PRIVATE void gto(ptr p)
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

void putLIN(long lin)
{
	dt_unprot(i2p(LINO_AD), (long)LINSIZE);
	dt_stn(i2p(LINO_AD), lin, (long)LINSIZE);
	LIN = lin;
	dt_prot(i2p(LINO_AD), (long)LINSIZE);
}

void putFIL(ptr fil)
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

PRIVATE void index_jump(size nbytes)
{
	register ptr cdp = dppop();	/* Case Descriptor Pointer */
	register long t_index =		/* Table INDEX */
			spop(nbytes) - mem_lds(cdp + psize, nbytes);
	register ptr nPC = 0;		/* New Program Counter */

	if (t_index >= 0 && t_index <= mem_lds(cdp + nbytes + psize, nbytes)) {
		nPC = mem_ldip(cdp + (2 * nbytes) + ((t_index + 1) * psize));
	}
	if (nPC == 0 && (nPC = mem_ldip(cdp)) == 0) {
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

PRIVATE void search_jump(size nbytes)
{
	register ptr cdp = dppop();	/* Case Descriptor Pointer */
	register long sv = spop(nbytes);/* Search Value */
	register long nt =		/* Number of Table-entries */
			mem_lds(cdp + psize, nbytes);
	register ptr nPC;		/* New Program Counter */

	while (--nt >= 0) {
		if (sv == mem_lds(cdp + (nt+1) * (nbytes+psize), nbytes)) {
			nPC = mem_ldip(cdp + nbytes + (nt+1)*(nbytes+psize));
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
PRIVATE void range_check(size nbytes)
{
	register ptr rdp = dppop();	/* Range check Descriptor Pointer */
	register long cv =		/* Check Value */
			st_lds(SP, nbytes);

	if (must_test && !(IgnMask&BIT(ERANGE))) {
		if (	cv < mem_lds(rdp, nbytes)
		||	cv > mem_lds(rdp + nbytes, nbytes)
		) {
			trap(ERANGE);
		}
	}
}
