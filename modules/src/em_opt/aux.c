#ifndef NORCSID
static char rcsid4[] = "$Header$";
#endif

#include "nopt.h"

arith
OO_rotate(w,amount)
	arith w, amount;
{
	long highmask, lowmask;
	highmask = (long)(-1) << amount;
	lowmask = ~highmask;
	if(OO_WSIZE!=4)
		highmask &= (OO_WSIZE==2)?0xFFFF:0xFF;
	return(((w<<amount)&highmask) | ((w >> (8*OO_WSIZE-amount))&lowmask));
}

int
OO_signsame(a,b)
	arith a, b;
{
	return( (a ^ b) >= 0);
}

int
OO_sfit(val,nbits)
	arith val, nbits;
{
	register long mask = ~((1L << (nbits - 1)) - 1);
	return(((val&mask) == 0) | (val&mask)==mask);
}

int
OO_ufit(val, nbits)
	arith val, nbits;
{
	return((val&(~((1L << (nbits - 1)) - 1))) == 0);
}

int
OO_extsame(a1,a2)
	register p_instr a1, a2;
{
	if (a1->em_argtype != a2->em_argtype)
		return(0);
	switch(a1->em_argtype) {
	case cst_ptyp:
		return (a1->em_cst == a2->em_cst);
	case sof_ptyp:
		if(a1->em_off != a2->em_off)
			return(0);
		return (strcmp(a1->em_dnam,a2->em_dnam)==0);
	case nof_ptyp:
		if (a1->em_off != a2->em_off)
			return(0);
		return (a1->em_dlb == a2->em_dlb);
	default:
		fatal("illegal type (%d) to sameext!",a1->em_argtype);
		/*NOTREACHED*/
	}
}

int
OO_namsame(a1,a2)
	register p_instr a1, a2;
{
	if (a1->em_argtype != a2->em_argtype)
		return(0);
	switch(a1->em_argtype) {
	case cst_ptyp:
		return 1;
	case sof_ptyp:
		return (strcmp(a1->em_dnam,a2->em_dnam)==0);
	case nof_ptyp:
		return (a1->em_dlb == a2->em_dlb);
	default:
		fatal("illegal type (%d) to samenam!",a1->em_argtype);
		/*NOTREACHED*/
	}
}

arith
OO_offset(a)
	register p_instr a;
{
	switch(a->em_argtype) {
	case cst_ptyp:
		return a->em_cst;
	case sof_ptyp:
		return a->em_off;
	case nof_ptyp:
		return a->em_off;
	default:
		fatal("illegal type (%d) to offset!",a->em_argtype);
		/*NOTREACHED*/
	}
}
