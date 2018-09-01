#include "rules.y"

gpr: GPR

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

u20
	: absexp
	{
		if (($1 < 0) || ($1 > 0xfffff))
			serror("20-bit unsigned value out of range");
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