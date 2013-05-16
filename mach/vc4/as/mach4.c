/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "binary.h"

operation
	: OP                                              { emit2($1); }

	| OP_ONEREG GPR
	{
		emit2($1 | ($2<<0));
	}

	| OP_ONELREG GPR
	{
		if ($2 >= 0x10)
			serror("cannot use r16+ here");
		emit2($1 | ($2<<0));
	}

	| OP_ALU GPR ',' GPR
	{
		emit2(B16(01000000, 00000000) | ($1<<8) | ($2<<0) | ($4<<4));
	}

	| OP_ALU GPR ',' '#' u5
	{
		if ($1 >= 0x10)
			serror("cannot use this ALU operation in 2op form");
		emit2(B16(01100000, 00000000) | ($1<<9) | ($2<<0) | ($5<<4));
	}
	;

e16
	: expr
	{
		DOTVAL += 2;
		newrelo($1.typ, RELO2 | FIXUPFLAGS);
		DOTVAL -= 2;
		$$ = $1.val & 0xFFFF;
	}
	;
		
u8
	: absexp
	{
		if (($1 < 0) || ($1 > 0xFF))
			serror("8-bit unsigned value out of range");
		$$ = $1;
	}
	;
	
u7
	: absexp
	{
		if (($1 < 0) || ($1 > 0x7F))
			serror("7-bit unsigned value out of range");
		$$ = $1;
	}
	;
	
u6
	: absexp
	{
		if (($1 < 0) || ($1 > 0x3F))
			serror("6-bit unsigned value out of range");
		$$ = $1;
	}
	;
	
u5
	: absexp
	{
		if (($1 < 0) || ($1 > 0x1F))
			serror("5-bit unsigned value out of range");
		$$ = $1;
	}
	;
	
u4
	: absexp
	{
		if (($1 < 0) || ($1 > 0xF))
			serror("4-bit unsigned value out of range");
		$$ = $1;
	}
	;
	
u1
	: absexp
	{
		if (($1 < 0) || ($1 > 1))
			serror("1-bit unsigned value out of range");
		$$ = $1;
	}
	;
	
u2
	: absexp
	{
		if (($1 < 0) || ($1 > 0x3))
			serror("2-bit unsigned value out of range");
		$$ = $1;
	}
	;

