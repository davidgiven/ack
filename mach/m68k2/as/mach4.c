/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)mach4.c	1.11 */
/*
 *  Motorola 68000/68010 syntax rules
 */

operation
	:	bcdx DREG ',' DREG
			{	emit2($1 | $2 | $4<<9);}
	|	bcdx '-' '(' AREG ')' ',' '-' '(' AREG ')'
			{	emit2($1 | $4 | $9<<9 | 010);}
	|	ADD sizedef ea_ea
			{	add($1, $2);}
	|	AND sizenon ea_ea
			{	and($1, $2);}
	|	SHIFT sizedef ea_ea
			{	shift_op($1, $2);}
	|	BR expr
			{	branch($1, $2);}
	|	DBR DREG ',' expr
			{	$4.val -= (DOTVAL+2);
				fit(fitw($4.val));
				emit2($1 | $2);
#ifdef RELOCATION
				newrelo($4.typ, RELPC|RELO2|RELBR|RELWR);
#endif
				emit2(loww($4.val));
			}
	|	BITOP ea_ea
			{	bitop($1);}
	|	OP_EA_D ea ',' DREG
			{	emit2($1 | mrg_2 | $4<<9);
				ea_2(SIZE_W, DTA);
			}
	|	LEA ea ',' AREG
			{	emit2(040700 | mrg_2 | $4<<9);
				ea_2(SIZE_L, CTR);
			}
	|	op_ea ea
			{	emit2(($1&0177700) | mrg_2);
				ea_2($1&0300, $1&017);
			}
	|	OP_NOOP
			{	emit2($1);}
	|	CMP sizedef ea_ea
			{	cmp($2);}
	|	MOVE sizenon ea_ea
			{	move($2);}
	|	MOVEP sizedef ea_ea
			{	movep($2);}
	|	MOVEM sizedef regs ',' notimmreg
			{	movem(0, $2, $3);}
	|	MOVEM sizedef notimmreg ',' regs
			{	movem(1, $2, $5);}
	| 	MOVES sizedef ea_ea
			{	test68010();
				if (mrg_1 <= 017) {
					emit2(007000 | $2 | mrg_2);
					emit2(mrg_1 << 12 | 04000);
					ea_2($2,ALT|MEM);
				} else if (mrg_2 <= 017) {
					emit2(007000 | $2 | mrg_1);
					emit2(mrg_2 << 12);
					ea_1($2,ALT|MEM);
				} else
					badoperand();
			}
	|	MOVEC creg ',' reg
			{	test68010();
				emit2(047172); emit2($2 | $4<<12);
			}
	|	MOVEC reg ',' creg
			{	test68010();
				emit2(047173); emit2($4 | $2<<12);
			}
	|	EXG reg ',' reg
			{	if (($2 & 010) == 0)
					emit2(
						(0140500|$4|$2<<9)
						+
						(($4&010)<<3)
					);
				else
					emit2(
						(0140600|$2|($4&07)<<9)
						-
						(($4&010)<<3)
					);
			}
	|	OP_EXT sizedef DREG
			{	checksize($2, 2|4); emit2(044000 | $2+0100 | $3);}
	|	SWAP DREG
			{	emit2(044100 | $2);}
	|	STOP imm
			{	emit2($1); ea_2(SIZE_W, 0);}
	|	LINK AREG ',' imm
			{	emit2(047120 | $2); ea_2(SIZE_W, 0);}
	|	UNLK AREG
			{	emit2(047130 | $2);}
	|	TRAP '#' absexp
			{	fit(fit4($3)); emit2(047100|low4($3));}
	|	RTD imm
			{	test68010();
				emit2(047164);
				ea_2(SIZE_W, 0);
			}
	|	MODEL
			{	model = $1;}
	;
bcdx	:	ABCD
	|	ADDX sizedef
			{	$$ = $1 | $2;}
	;
creg	:	CREG
	|	SPEC	{	if ($1 != 075) badoperand(); $$ = 04000;}
	;
op_ea	:	OP_EA
	|	SZ_EA sizedef
			{	$$ = $1 | $2;}
	;
regs	:	rrange
	|	regs '/' rrange
			{	$$ = $1 | $3;}
	;
rrange	:	reg
			{	$$ = 1<<$1;}
	|	reg '-' reg
			{	if ($1 > $3)
					badoperand();
				for ($$ = 0; $1 <= $3; $1++)
					$$ |= (1<<$1);
			}
	;
ea	:	DREG
			{	mrg_2 = $1;}
	|	AREG
			{	mrg_2 = 010 | $1;}
	|	SPEC
			{	mrg_2 = $1;}
	|	notimmreg
	|	imm
	;
notimmreg
	:	'(' AREG ')'
			{	mrg_2 = 020 | $2;}
	|	'(' AREG ')' '+'
			{	mrg_2 = 030 | $2;}
	|	'-' '(' AREG ')'
			{	mrg_2 = 040 | $3;}
	|	expr sizenon
			{	exp_2 = $1; ea707172($2);
				RELOMOVE(rel_2, relonami);
			}
	|	expr '(' reg sizenon ')'
			{	exp_2 = $1; ea5x73($3, $4);
				RELOMOVE(rel_2, relonami);
			}
	|	expr '(' AREG ',' reg sizedef ')'
			{	exp_2 = $1; ea6x($3, $5, $6);
				RELOMOVE(rel_2, relonami);
			}
	|	expr '(' PC ')'
			{	exp_2 = $1; ea72();
				RELOMOVE(rel_2, relonami);
			}
	|	expr '(' PC ',' reg sizedef ')'
			{	exp_2 = $1; ea73($5, $6);
				RELOMOVE(rel_2, relonami);
			}
	;
imm	:	'#' expr
			{	mrg_2 = 074; exp_2 = $2;
				RELOMOVE(rel_2, relonami);
			}
	;
reg	:	DREG
	|	AREG
			{	$$ = $1 | 010;}
	;
sizedef	:	/* empty */
			{	$$ = SIZE_DEF;}
	|	SIZE
	;
sizenon	:	/* empty */
			{	$$ = SIZE_NON;}
	|	SIZE
	;
ea_ea	:	ea ','
			{	mrg_1 = mrg_2; exp_1 = exp_2;
				RELOMOVE(rel_1, rel_2);
			}
		ea
	;
