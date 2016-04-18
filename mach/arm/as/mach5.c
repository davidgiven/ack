#include <stdint.h>

#define maskx(v, x) (v & ((1<<(x))-1))

void branch(word_t brtyp, word_t link, struct expr_t* expr)
{
	uint32_t pc = DOTVAL;
	uint32_t type = expr->typ & S_TYP;
	int d;

	/* Sanity checking. */

	if (type == S_ABS)
		serror("can't use absolute addresses here");
	
	/* Calculate the instruction displacement. */

	d = (int32_t)expr->val - (int32_t)pc;
	if ((pass == 2) && (d > 0) && !(expr->typ & S_DOT))
		d -= DOTGAIN;
	d = (d - 8) >> 2;

	if (type != DOTTYP)
		newrelo(expr->typ, RELOARM|RELPC);

	emit4(brtyp|link | maskx(d, 24));
}

data(opc, ins, val, typ)
long opc, ins;
valu_t val;
short typ;
{
	valu_t tmpval;
	int adrflag = 0;

	if (typ == S_REG){	/* The argument is a register */
		emit4(opc|ins|val);
		return;
	}

/* Do a bit of optimisation here, since the backend might produce instructions
   of the type    MOV R0, R0, #0.   We can ignore these. */

	if (((opc == ADD) || (opc == SUB)) && (val == 0)){	/* ADD or SUB 0 ? */
		if ((ins & 0x000F0000) == ((ins & 0x0000F000) << 4)) /* Same reg ? */
			return;		/* Don't emit anything */
	}

/* No optimisation, so carry on ... */

	ins |= 0x02000000;	/* The argument is an immediate value */
	tmpval = val;
	if (opc == 0xff){		/* This is an ADR */
		adrflag = 1;
		opc = MOV;
	}

	if (typ == S_ABS){	/* An absolute value */
		if (calcimm(&opc, &tmpval, typ)){
			emit4(opc|ins|tmpval);
			return;
		}
	}

	tmpval = val;
	if (!adrflag){		/* Don't do this for ADRs */
		if (oursmall(calcimm(&opc, &tmpval, typ), 12)){
			emit4(opc|ins|tmpval);
			return;
		}	
	}

	if (opc == MOV || opc == MVN || opc == ADD || opc == SUB){
		if (!bflag && pass == PASS_3){		/* Debugging info */
			/* warning("MOV/ADD extension"); */
			/* if (dflag)
				printf("value: %lx\n", val);*/
		}
		if (oursmall((val & 0xFFFF0000) == 0, 8)){
			putaddr(opc, ins, val, 2);
			return;
		}
		if (oursmall((val & 0xFF000000) == 0, 4)){
			putaddr(opc, ins, val, 3);
			return;
		}
		putaddr(opc, ins, val, 4);
		return;
	}
	
	if (pass == PASS_1)
		DOTVAL += 16;	/* Worst case we can emit */
	else
		serror("immediate value out of range");
	return;
}


/* Calculate an immediate value.  This is not as easy as it sounds, because
   the ARM uses an 8-bit value and 4-bit shift to encode the value into a 
   12-bit field.  Unfortunately this means that some numbers may not fit at
   all. */

calcimm(opc,val,typ)
word_t *opc;
valu_t *val;
short typ;
{
	int i = 0;

	if (typ == S_UND)
		return(0);	/* Can't do anything with an undefined label */

	if ((*val & 0xFFFFFF00) == 0)	/* Value is positive, but < 256, */
		return(1); 		/* so doesn't need a shift */

	if ((~*val & 0xFFFFFF00) == 0){	/* Value is negative, but < 256, */
		if (*opc == AND)	/* so no shift required, only */
			{		/* inversion */
			*val = ~*val;
			*opc = BIC;
			return(1);
			}
		if (*opc == MOV)
			{
			*val = ~*val;
			*opc = MVN;
			return(1);
			}
		if (*opc == ADC)
			{
			*val = ~*val;
			*opc = SBC;
			return(1);
			}

	}	
	if ((-1**val & 0xFFFFFF00) == 0){ /* Same idea ... */
		if (*opc == ADD)
			{
			*val *= -1;
			*opc = SUB;
			return(1);
			}
		if (*opc == CMP)
			{
			*val *= -1;
			*opc = CMN;
			return(1);
			}
	}

	do{					/* Now we need to shift */
		rotateleft2(&*val);		/* Rotate left by two bits */
		i++;
		if((*val & 0xFFFFFF00) == 0){	/* Got a value < 256 */
			*val = *val|i<<8;	/* OR in the shift */
			return(1);
		}
		if ((~*val & 0xFFFFFF00) == 0){ /* If negative, carry out */
			if (*opc == AND)	/* inversion as before */
				{
				*val = ~*val|i<<8;
				*opc = BIC;
				return(1);
				}
			if (*opc == MOV)
				{
				*val = ~*val|i<<8;
				*opc = MVN;
				return(1);
				}
			if (*opc == ADC)
				{
				*val = ~*val|i<<8;
				*opc = SBC;
				return(1);
				}
		}	
	}while(i<15);

	return(0);	/* Failed if can't encode it after 16 rotates */
}


/* Calculate an offset in an address */

word_t calcoffset(valu_t val)
{
	if (fitu(val, 12))
		return val|0x00800000;
	val = -val;
	if (fitu(val, 12))
		return val;

	serror("offset out of range");
	return 0;
}

/* Calculate a split offset (for strh and friends). */

word_t splitoffset(valu_t val)
{
	if (fitu(val, 8))
		return (val&0x0f) | ((val&0xf0)<<4) | 0x00800000;
	val = -val;
	if (fitu(val, 8))
		return (val&0x0f) | ((val&0xf0)<<4) | 0x00800000;

	serror("offset out of range");
	return 0;
}

/* This routine deals with STR and LDR instructions */

void strldr(uint32_t opc, valu_t val)
{
	uint32_t d;

/* If the expression was a register, then just output it and save 24
   bytes */

	if (success)
	{ 
		emit4(opc|val);
		return;
	}

	d = val - DOTVAL - 8;
	if (fitu(d, 12))
	{	/* If it's +ve */
		emit4(opc|d|0x018F0000);	/* PC rel, up bit */
		return;
	}

	d = -d;
	if (fitu(d, 12))
	{	/* If it's -ve */
		emit4(opc|d|0x010F0000);	/* PC rel, no up bit */
		return;
	}

	serror("displacement overflow: 0x%x", d);
	return;
}


/* This routine deals with ADR instructions.  The ARM does not have a
   'calculate effective address' instruction, so we use ADD, SUB, MOV or
   MVN instead.  ADR is not a genuine instruction, but is provided to make
   life easier.  At present these are all calculated by using a MOV and
   successive ADDs.  Even if the address will fit into a single MOV, we 
   still use two instructions; the second is a no-op.  This is to cure the
   optimisation problem with mobile addresses ! */


calcadr(ins, reg, val, typ)
word_t ins, reg;
valu_t val;
short typ;
{
	valu_t tmpval = val;
	word_t opc = 0xff;	/* Dummy opc used as a flag for data() */

/* First check that the address is in range */

	if (val < 0) 
		tmpval = ~tmpval; /* Invert negative addresses for check */

	if ((tmpval & 0xFC000000) && (typ != S_UND)){
		serror("adr address out of range");
		return;
	}

/* Can't do it PC relative, so use an absolute MOV instead */

	data (opc, ins|reg<<12, val, typ);
	return; 

}


word_t
calcshft(val, typ, styp)
valu_t val;
short typ;
word_t styp;
{
	if (typ == S_UND) 
		return(0);

	if (val & 0xFFFFFFE0) 
		serror("shiftcount out of range");

	if (styp && !val) 
		warning("shiftcount 0");

	return((val & 0x1F)<<7);
}

rotateleft2(x)
long *x;
{
	unsigned long bits;

	bits = *x & 0xC0000000;
	*x <<= 2 ;
	if (bits){
		bits >>= 30;
		*x |= bits;
	}
	return;
}


/* 
   This routine overcomes the 12-bit encoding problem by outputting a number
   a byte at a time.  For a MOV, it first uses a MOV, then successive ADDs.  
   It will not use any more ADDs than needed to completely output the number.  
   A similar approach is used for ADDs and SUBs.
   There is a problem here with optimisation in the third pass; if the 
   instruction needed two ADDs in the second pass, but only one in the third 
   pass, then the second ADD is replaced with a no-op.  We cannot emit one 
   less instruction, because that will upset other addresses.
*/

putaddr(opc, ins, val, count)
long opc, ins, val;			
int count;
{
	long tmpval = val;
	long reg = ins & 0x0000F000;

	emit4(opc|ins|(val & 0x000000FF));

	tmpval = (val & 0x0000FF00) >> 8 | 0x00000C00;

/* Decide what to use for the additional instructions */

	if (opc == 0x03a00000)		/* This one is for strldr */
		opc = 0x02800000;

	if (opc == MOV)
		opc = ADD;

	if (opc == MVN)
		opc = SUB;

	if ((tmpval & 0x000000FF) != 0)
		emit4(opc|ins|reg<<4|tmpval);
	else
		emit4(0xF0000000);	/* No-op if a zero argument */

	if (count == 3 || count == 4){	/* Must use three or more instructions */
		if ((val & 0xFFFF0000) != 0){
			tmpval = (val & 0x00FF0000) >> 16 | 0x00000800;
			emit4(opc|ins|reg<<4|tmpval);
		}
		else 
			emit4(0xF0000000);		/* No-op */
	}

	if (count == 4){	/* Must use four instructions */
		if ((val & 0xFF000000) != 0){
			tmpval = (val & 0xFF000000) >> 24 | 0x00000400;
			emit4(opc|ins|reg<<4|tmpval);
		}
		else 
			emit4(0xF0000000);		/* No-op */
	}

	return;
}


/* The following piece of code is stolen from comm7.c; it needs some minor 
   fixes for the ARM, so it is included here rather than altering the existing
   code.  It maintains a bit table to say whether or not an optimisation is
   possible.  The original had some problems:
   (a).	  It assumed that the memory returned by malloc() was cleared to zero.
          This is true on a Sun, but not under Minix; small() should really
	  use calloc() instead.
   (b).   It assumed that if an optimisation was possible in pass 2, it must
          also be possible in pass 3, and produced an assertion error if it
	  wasn't.   This is OK for optimising things like long or short branch
	  instructions on a 68000, but not for ADRs on the ARM.  A previous
	  optimisation may place an address out of 12-bit encoding range on
	  pass 3, when it was in range on pass 2.  However we have to be 
	  careful here .....
 */

#define PBITTABSZ	128
static char *pbittab[PBITTABSZ];

oursmall(fitsmall, gain)
{
	register bit;
	register char *p;

	if (DOTSCT == NULL)
		nosect();
	if (bflag)
		return(0);
	if (nbits == BITCHUNK) {
		bitindex++;
		nbits = 0;
		if (bitindex == PBITTABSZ) {
			static int w_given;
			if (pass == PASS_1 && ! w_given) {
				w_given = 1;
				warning("bit table overflow");
			}
			return(0);
		}
		if (pbittab[bitindex] == 0 && pass == PASS_1) {
			if ((pbittab[bitindex] = malloc(MEMINCR)) == 0) {
				static int w2_given;

				if (!w2_given) {
					w2_given = 1;
					warning("out of space for bit table");
				}
			}
		}
		if (pbittab[bitindex] == 0)
			return (0);
	}
	bit = 1 << (nbits&7);
	p = pbittab[bitindex]+(nbits>>3);
	nbits++;

	switch (pass) {
	case PASS_1:
		*p = 0;
		return(0);
	case PASS_2:
		if (fitsmall) {
			DOTGAIN += gain;
			*p |= bit;
		}
		return(fitsmall);
	case PASS_3:
		if (!(fitsmall || (*p & bit) == 0)){
			printf("line: %ld - small failed\n", lineno);
			printf("fitsmall: %d bit: %d\n", fitsmall, (*p & bit));
			if (fitsmall) 
				return(0);
			else
				serror("This one is fatal!");
		}
		return(*p & bit);
	}
	/*NOTREACHED*/
}
