/* $Header$ */
#include "nopt.h"

OO_rotate(w,amount)
	int w, amount;
{
	long highmask, lowmask;
	highmask = (long)(-1) << amount;
	lowmask = ~highmask;
	if(OO_WSIZE!=4)
		highmask &= (OO_WSIZE==2)?0xFFFF:0xFF;
	return(((w<<amount)&highmask) | ((w >> (8*OO_WSIZE-amount))&lowmask));
}

OO_signsame(a,b)
	int a, b;
{
	return( (a ^ b) >= 0);
}

OO_sfit(val,nbits)
	int val, nbits;
{
	long mask = 0;
	int i;
	for(i=nbits-1;i<8*sizeof(mask);i++)
		mask |= (1<<i);
	return(((val&mask) == 0) | (val&mask)==mask);
}

OO_ufit(val, nbits)
	int val, nbits;
{
	long mask = 0;
	int i;
	for(i=nbits;i<8*sizeof(mask);i++)
		mask |= (1<<i);
	return((val&mask) == 0);
}

OO_extsame(a1,a2)
	struct instr *a1, *a2;
{
	if(a1->argtype != a2->argtype) return(0);
	switch(a1->argtype) {
	case cst_ptyp:
		return(a1->acst == a2->acst);
	case sof_ptyp:
		if(a1->asoff != a2->asoff) return(0);
		return(strcmp(a1->adnam,a2->adnam)==0);
	case nof_ptyp:
		if(a1->anoff != a2->anoff) return(0);
		return(a1->adlb == a2->adlb);
	default:
		fatal("illegal type (%d) to sameext!",a1->argtype);
	}
}

OO_namsame(a1,a2)
	struct instr *a1, *a2;
{
	if(a1->argtype != a2->argtype) return(0);
	switch(a1->argtype) {
	case cst_ptyp:
		return(1);
	case sof_ptyp:
		return(strcmp(a1->adnam,a2->adnam)==0);
	case nof_ptyp:
		return(a1->adlb == a2->adlb);
	default:
		fatal("illegal type (%d) to samenam!",a1->argtype);
	}
}

OO_offset(a)
	struct instr *a;
{
	switch(a->argtype) {
	case cst_ptyp:
		return(a->acst);
	case sof_ptyp:
		return(a->asoff);
	case nof_ptyp:
		return(a->anoff);
	default:
		fatal("illegal type (%d) to offset!",a->argtype);
	}
}
