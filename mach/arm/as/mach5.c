#include <stdint.h>

#define maskx(v, x) (v & ((1 << (x)) - 1))

static int calcimm(word_t* opc, valu_t* val, short typ);

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
		newrelo(expr->typ, RELOARM | RELPC);

	emit4(brtyp | link | maskx(d, 24));
}

void data(long opc, long ins, valu_t val, short typ)
{
	valu_t tmpval;

	if (typ == S_REG)
	{ /* The argument is a register */
		emit4(opc | ins | val);
		return;
	}

	ins |= 0x02000000; /* The argument is an immediate value */
	tmpval = val;

	if (typ == S_ABS)
	{ /* An absolute value */
		if (calcimm(&opc, &tmpval, typ))
		{
			emit4(opc | ins | tmpval);
			return;
		}
	}

	tmpval = val;
	if (oursmall(calcimm(&opc, &tmpval, typ), 12))
	{
		emit4(opc | ins | tmpval);
		return;
	}

	if (pass == PASS_1)
		DOTVAL += 4; /* Worst case we can emit */
	else
		serror("immediate value out of range");
	return;
}

/* Calculate an immediate value.  This is not as easy as it sounds, because
   the ARM uses an 8-bit value and 4-bit shift to encode the value into a 
   12-bit field.  Unfortunately this means that some numbers may not fit at
   all. */

static int calcimm(word_t* opc, valu_t* val, short typ)
{
	int i = 0;

	if (typ == S_UND)
		return (0); /* Can't do anything with an undefined label */

	if ((*val & 0xFFFFFF00) == 0) /* Value is positive, but < 256, */
		return (1); /* so doesn't need a shift */

	if ((~*val & 0xFFFFFF00) == 0)
	{ /* Value is negative, but < 256, */
		if (*opc == AND) /* so no shift required, only */
		{ /* inversion */
			*val = ~*val;
			*opc = BIC;
			return (1);
		}
		if (*opc == MOV)
		{
			*val = ~*val;
			*opc = MVN;
			return (1);
		}
		if (*opc == ADC)
		{
			*val = ~*val;
			*opc = SBC;
			return (1);
		}
	}
	if ((-1 * *val & 0xFFFFFF00) == 0)
	{ /* Same idea ... */
		if (*opc == ADD)
		{
			*val *= -1;
			*opc = SUB;
			return (1);
		}
		if (*opc == CMP)
		{
			*val *= -1;
			*opc = CMN;
			return (1);
		}
	}

	do
	{ /* Now we need to shift */
		rotateleft2(&*val); /* Rotate left by two bits */
		i++;
		if ((*val & 0xFFFFFF00) == 0)
		{ /* Got a value < 256 */
			*val = *val | i << 8; /* OR in the shift */
			return (1);
		}
		if ((~*val & 0xFFFFFF00) == 0)
		{ /* If negative, carry out */
			if (*opc == AND) /* inversion as before */
			{
				*val = ~*val | i << 8;
				*opc = BIC;
				return (1);
			}
			if (*opc == MOV)
			{
				*val = ~*val | i << 8;
				*opc = MVN;
				return (1);
			}
			if (*opc == ADC)
			{
				*val = ~*val | i << 8;
				*opc = SBC;
				return (1);
			}
		}
	} while (i < 15);

	return (0); /* Failed if can't encode it after 16 rotates */
}

/* Calculate an offset in an address */

word_t calcoffset(valu_t val)
{
	if (fitu(val, 12))
		return val | 0x00800000;
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
		return (val & 0x0f) | ((val & 0xf0) << 4) | 0x00800000;
	val = -val;
	if (fitu(val, 8))
		return (val & 0x0f) | ((val & 0xf0) << 4) | 0x00800000;

	serror("offset out of range");
	return 0;
}

void strldr(uint32_t opc, struct expr_t* expr)
{
	quad type = expr->typ & S_TYP;
	uint32_t d;
	
	/* Sanity checking. */

	if (type == S_ABS)
		serror("can't use absolute addresses here");
	
	if (type == DOTTYP)
	{
		/* Reference to code in this section. */

		uint32_t d = expr->val - DOTVAL - 8;
		if (fitu(d, 12))
		{ /* If it's +ve */
			emit4(opc | d | 0x018F0000); /* PC rel, up bit */
			return;
		}

		d = -d;
		if (fitu(d, 12))
		{ /* If it's -ve */
			emit4(opc | d | 0x010F0000); /* PC rel, no up bit */
			return;
		}

		serror("displacement to near constant is too big");
	}

	serror("cannot directly address symbols in another section");
}

word_t calcshft(valu_t val, short typ, word_t styp)
{
	if (typ == S_UND)
		return (0);

	if (val & 0xFFFFFFE0)
		serror("shiftcount out of range");

	if (styp && !val)
		warning("shiftcount 0");

	return ((val & 0x1F) << 7);
}

void rotateleft2(long* x)
{
	unsigned long bits;

	bits = *x & 0xC0000000;
	*x <<= 2;
	if (bits)
	{
		bits >>= 30;
		*x |= bits;
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

#define PBITTABSZ 128
static char* pbittab[PBITTABSZ];

int oursmall(int fitsmall, int gain)
{
	register bit;
	register char* p;

	if (DOTSCT == NULL)
		nosect();
	if (bflag)
		return (0);
	if (nbits == BITCHUNK)
	{
		bitindex++;
		nbits = 0;
		if (bitindex == PBITTABSZ)
		{
			static int w_given;
			if (pass == PASS_1 && !w_given)
			{
				w_given = 1;
				warning("bit table overflow");
			}
			return (0);
		}
		if (pbittab[bitindex] == 0 && pass == PASS_1)
		{
			if ((pbittab[bitindex] = malloc(MEMINCR)) == 0)
			{
				static int w2_given;

				if (!w2_given)
				{
					w2_given = 1;
					warning("out of space for bit table");
				}
			}
		}
		if (pbittab[bitindex] == 0)
			return (0);
	}
	bit = 1 << (nbits & 7);
	p = pbittab[bitindex] + (nbits >> 3);
	nbits++;

	switch (pass)
	{
		case PASS_1:
			*p = 0;
			return (0);
		case PASS_2:
			if (fitsmall)
			{
				DOTGAIN += gain;
				*p |= bit;
			}
			return (fitsmall);
		case PASS_3:
			if (!(fitsmall || (*p & bit) == 0))
			{
				printf("line: %ld - small failed\n", lineno);
				printf("fitsmall: %d bit: %d\n", fitsmall, (*p & bit));
				if (fitsmall)
					return (0);
				else
					serror("This one is fatal!");
			}
			return (*p & bit);
	}
	/*NOTREACHED*/
}
