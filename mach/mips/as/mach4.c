#include "rules.y"
	| OP_LI GPR ',' expr
	{
		word_t reg = $2;
		word_t type = $4.typ & S_TYP;
		word_t val = $4.val;
		if (type != S_ABS)
			serror("li cannot be used with values that need a fixup");
		
		if (val == 0)
			emit4(0x00000025 | (reg<<11)); /* or reg, zero, zero */
		else if ((val < -0x8000) || (val > 0xffff))
			emit4(0x24000000 | (reg<<16) | (val & 0xffff)); /* addiu reg, zero, value */
		else
		{
			emit4(0x3c000000 | (reg<<16) | (val>>16)); /* lui reg, value */
			emit4(0x34000000 | (reg<<16) | (reg<<21) | (val & 0xffff)); /* ori reg, reg, value */
		}
	}
	| OP_LA GPR ',' expr
	{
		word_t reg = $2;
		word_t type = $4.typ & S_TYP;
		word_t val = $4.val;

		if (type != S_ABS)
			newrelo($4.typ, RELOMIPSHI | FIXUPFLAGS);
		emit4(0x3c000000 | (reg<<16) | (val>>16)); /* lui reg, value */
		if (type != S_ABS)
			newrelo($4.typ, RELOMIPS | FIXUPFLAGS);
		emit4(0x34000000 | (reg<<16) | (reg<<21) | (val & 0xffff)); /* ori reg, reg, value */
	}

gpr: GPR
fpr: FPR

e16
	: absexp
	{
		/* Allow signed or unsigned 16-bit values. */
		if (($1 < -0x8000) || ($1 > 0xffff))
			serror("16-bit signed value out of range");
		$$ = (uint16_t) $1;
	}
	;

e9
	: absexp
	{
		/* Allow signed or unsigned 9-bit values. */
		if (($1 < -0x100) || ($1 > 0x1ff))
			serror("9-bit signed value out of range");
		$$ = (uint16_t) $1;
	}
	;

u25
	: absexp
	{
		if (($1 < 0) || ($1 > 0x1ffffff))
			serror("25-bit unsigned value out of range");
		$$ = $1;
	}
	;

u20
	: absexp
	{
		if (($1 < 0) || ($1 > 0xfffff))
			serror("20-bit unsigned value out of range");
		$$ = $1;
	}
	;

u16
	: absexp
	{
		if (($1 < 0) || ($1 > 0xffff))
			serror("16-bit unsigned value out of range");
		$$ = $1;
	}
	;

u5
	: absexp
	{
		if (($1 < 0) || ($1 > 0x1f))
			serror("5-bit unsigned value out of range");
		$$ = $1;
	}
	;

u3
	: absexp
	{
		if (($1 < 0) || ($1 > 0x7))
			serror("3-bit unsigned value out of range");
		$$ = $1;
	}
	;

offset16
	: expr
	{
		int dist = $1.val - DOTVAL;
		fit(fitx(dist, 18));

		if (dist & 0x3)
			serror("jump targets must be 4-aligned");

		newrelo($1.typ, RELOMIPS | RELPC | FIXUPFLAGS);
		$$ = (dist >> 2) & 0x0000ffff;
	}
	;

abs26
	: expr
	{
		int target = $1.val;
		fit(fitx(target, 28));

		if (target & 0x3)
			serror("jump targets must be 4-aligned");

		newrelo($1.typ, RELOMIPS | FIXUPFLAGS);
		$$ = (target >> 2) & 0x03fffffd;
	}
	;

extmsblsb
	: u5 ',' u5
	{
		int pos = $1;
		int size = $3;
		$$ = ((size-1) << 5) | pos;
	}
	;

insmsblsb
	: u5 ',' u5
	{
		int pos = $1;
		int size = $3;
		$$ = ((pos+size-1) << 5) | pos;
	}
	;