/* $Header$ */
#include "nopt.h"

rotate(w,amount)
	int w, amount;
{
	long highmask, lowmask;
	highmask = (long)(-1) << amount;
	lowmask = ~highmask;
	if(WSIZE!=4)
		highmask &= (WSIZE==2)?0xFFFF:0xFF;
	return(((w<<amount)&highmask) | ((w >> (8*WSIZE-amount))&lowmask));
}

samesign(a,b)
	int a, b;
{
	return( (a ^ b) >= 0);
}

sfit(val,nbits)
	int val, nbits;
{
	long mask = 0;
	int i;
	for(i=nbits-1;i<8*sizeof(mask);i++)
		mask |= (1<<i);
	return(((val&mask) == 0) | (val&mask)==mask);
}

ufit(val, nbits)
	int val, nbits;
{
	long mask = 0;
	int i;
	for(i=nbits;i<8*sizeof(mask);i++)
		mask |= (1<<i);
	return((val&mask) == 0);
}

sameext(a1,a2)
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

samenam(a1,a2)
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

offset(a)
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
