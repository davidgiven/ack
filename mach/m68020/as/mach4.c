/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 *  Motorola 68020 syntax rules
 */

operation
	:		{	instrp = instr;
				dot_offset = 0;
			}
		instruction
			{	emit_instr();
			}
	;
instruction
	:	bcdx DREG ',' DREG
			{	T_EMIT2($1 | $2 | $4<<9,0,0,0);}
	|	bcdx '-' '(' AREG ')' ',' '-' '(' AREG ')'
			{	T_EMIT2($1 | $4 | $9<<9 | 010,0,0,0);}
	|	ADD sizedef ea_ea
			{	add($1, $2);}
	|	AND sizenon ea_ea
			{	and($1, $2);}
	|	SHIFT sizedef ea_ea
			{	shift_op($1, $2);}
	|	SHIFT sizedef ea	/* This syntax is also allowed */
			{	checksize($2, 2);
				T_EMIT2(($1 & 0177700) | mrg_2,0,0,0);
				ea_2(SIZE_W, MEM|ALT);
			}
	|	BR expr
			{	branch($1, $2);}
	|	DBR DREG ',' expr
			{	T_EMIT2($1 | $2,0,0,0);
				$4.val -= (DOTVAL+dot_offset);
				fit(fitw($4.val));
				T_EMIT2(loww($4.val), $4.typ,
						RELPC|RELO2, relonami);
			}
	|	BITOP ea_ea
			{	bitop($1);}
	|	BITFIELD ea off_width
			{	bitfield($1, $3);}
	|	BF_TO_D ea off_width ',' DREG
			{	bitfield($1, $3 | $5<<12);}
	|	BFINS DREG ',' ea off_width
			{	bitfield($1, $5 | $2<<12);}
	|	DIVMUL sizedef ea ',' DREG
			{	checksize($2, 2|4);
				if ($2 == SIZE_W) {
					T_EMIT2((0140300^($1<<8))|mrg_2|$5<<9,
								0, 0, 0);
					ea_2(SIZE_W, DTA);
				}
				else {	/* 32 bit dividend or product */
					T_EMIT2((046000 | ($1 & ~1)) | mrg_2,
								0, 0, 0);
					T_EMIT2(($1&1)<<11 | $5<<12 | $5,
								0, 0, 0);
					ea_2(SIZE_L, DTA);
				}
			}
	|	DIVMUL sizedef ea ',' DREG ':' DREG
			{		/* 64 bit dividend or product */
				checksize($2, 4);
				T_EMIT2((046000 | ($1 & ~1)) | mrg_2, 0, 0, 0);
				T_EMIT2(($1&1)<<11 | $7<<12 | $5 ,0,0,0);
				ea_2(SIZE_L, DTA);
			}
	|	DIVL sizedef ea ',' DREG ':' DREG
			{	    /* 32 bit long division with remainder */
				checksize($2, 4);
				T_EMIT2(($1 & ~1) | mrg_2, 0, 0, 0);
				T_EMIT2(($1 & 1)<<11 | $7<<12 | $5 | 02000, 0, 0, 0);
				ea_2(SIZE_L, DTA);
			}
	|	LEA ea ',' AREG
			{	T_EMIT2(040700 | mrg_2 | $4<<9,0,0,0);
				ea_2(SIZE_L, CTR);
			}
	|	op_ea ea
			{	if (mrg_2==074)
					serror("bad adressing category");
				T_EMIT2(($1&0177700) | mrg_2,0,0,0);
				ea_2($1&0300, $1&017);
			}
	|	OP_NOOP
			{	T_EMIT2($1,0,0,0);}
	|	OP_EXT SIZE DREG
			{	checksize($2, ($1 & 0400) ? 4 : (2|4));
				T_EMIT2($1 | $2+0100 | $3,0,0,0);
			}
	|	OP_RANGE sizedef ea ',' reg
			{	T_EMIT2(0300 | ($2<<3) | mrg_2,0,0,0);
				T_EMIT2($1 | ($5<<12),0,0,0);
				ea_2($2, CTR);
			}
	|	TRAPCC SIZE imm
			{	checksize($2, 2|4);
				T_EMIT2($1 | ($2>>6)+1,0,0,0);
				ea_2($2, 0);
			}
	|	TRAPCC	{	T_EMIT2($1 | 4,0,0,0);}
	|	PACK '-' '(' AREG ')' ',' '-' '(' AREG ')' ',' imm
			{	T_EMIT2($1 | 8 | $4 | $9<<9, 0, 0, 0);
				ea_2(SIZE_W, 0);
			}
	|	PACK DREG ',' DREG ',' imm
			{	T_EMIT2($1 | $2 | $4<<9, 0, 0, 0);
				ea_2(SIZE_W, 0);
			}
	|	CMP sizedef ea_ea
			{	cmp($2);}
	|	CHK sizedef ea ',' DREG
			{	checksize($2, 2|4);
				T_EMIT2(040000 | mrg_2 | $5<<9 |
						($2==SIZE_W ? 0600 : 0400),
								0, 0, 0);
				ea_2($2, DTA);
			}
	|	MOVE sizenon ea_ea
			{	move($2);}
	|	MOVEP sizedef ea_ea
			{	movep($2);}
	|	MOVEM sizedef regs ',' notimmreg
			{	movem(0, $2, $3);}
	|	MOVEM sizedef notimmreg ',' regs
			{	movem(1, $2, $5);}
	| 	MOVES sizedef ea_ea
			{	if (mrg_1 <= 017) {
					T_EMIT2(007000 | $2 | mrg_2,0,0,0);
					T_EMIT2(mrg_1 << 12 | 04000,0,0,0);
					ea_2($2,ALT|MEM);
				} else if (mrg_2 <= 017) {
					T_EMIT2(007000 | $2 | mrg_1,0,0,0);
					T_EMIT2(mrg_2 << 12,0,0,0);
					ea_1($2,ALT|MEM);
				} else
					badoperand();
			}
	|	MOVEC creg ',' reg
			{	T_EMIT2(047172,0,0,0);
				T_EMIT2($2 | $4<<12,0,0,0);
			}
	|	MOVEC reg ',' creg
			{	T_EMIT2(047173,0,0,0);
				T_EMIT2($4 | $2<<12,0,0,0);
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
						(0140610|$2|($4&07)<<9)
						-
						(($4&010)<<3)
					);
			}
	|	SWAP DREG
			{	T_EMIT2(044100 | $2,0,0,0);}
	|	STOP imm
			{	T_EMIT2(047162, 0, 0, 0);
				ea_2(SIZE_W, 0);
			}
	|	LINK sizenon AREG ',' imm
			{	link($2, $3);}
	|	UNLK AREG
			{	T_EMIT2(047130 | $2,0,0,0);}
	|	TRAP '#' absexp
			{	fit(fit4($3));
				T_EMIT2(047100|low4($3),0,0,0);
			}
	|	RTD imm
			{	T_EMIT2(047164,0,0,0);
				ea_2(SIZE_W, 0);
			}
	|	BKPT '#' absexp
			{	fit(($3 & ~07) == 0);
				T_EMIT2(044110 | low3($3),0,0,0);
			}
	|	CALLM '#' absexp ',' ea
			{	fit(fitb($3));
				T_EMIT2(03300 | mrg_2,0,0,0);
				T_EMIT2($3,0,0,0);
				ea_2(SIZE_L, CTR);
			}
	|	RTM reg
			{	T_EMIT2(03300 | $2, 0, 0, 0);}
	|	CAS sizedef DREG ',' DREG ',' ea
			{	T_EMIT2(04300 | (($2+0100)<<3) | mrg_2,0,0,0);
				T_EMIT2($5 | ($3<<6),0,0,0);
				ea_2($2, MEM|ALT);
			}
	|	CAS2 sizedef DREG ':' DREG ',' DREG ':' DREG ','
					'(' reg ')' ':' '(' reg ')'
			{	checksize($2 , 2|4);
				T_EMIT2(04374 | (($2+0100)<<3),0,0,0);
				T_EMIT2($3 | ($7<<6) | ($12<<12),0,0,0);
				T_EMIT2($5 | ($9<<6) | ($16<<12),0,0,0);
			}
	|   /* Coprocessor instructions; syntax may be changed (please).
	     * No coprocessor defined extension words are emitted.
	     */
		CP CPBCC cp_cond expr
			{	cpbcc($2 | $1 | $3, $4);
			}
	|	CP CPDBCC cp_cond DREG ',' expr
			{	T_EMIT2($2 | $1 | $4,0,0,0);
				$6.val -= (DOTVAL+dot_offset);
				fit(fitw($6.val));
				T_EMIT2(loww($6.val), $6.typ,
						RELPC|RELO2, relonami);
			}
	|	CP CPGEN
			{	T_EMIT2($2 | $1,0,0,0);
				/* NO COMMAND WORD IS EMITTED;
				 * THIS INSTRUCTIONS IS (STILL) ONE BIG RIDDLE.
				 * NO EFFECTIVE ADDRESS IS CALCULATED (SYNTAX ?)
				 */
			}
	|	CP CPRESTORE ea
			{	T_EMIT2($2 | $1 | mrg_2,0,0,0);
				ea_2(SIZE_W, (mrg_2 & 070)==030 ? 0 : CTR);
			}
	|	CP CPSAVE ea
			{	T_EMIT2($2 | $1 | mrg_2,0,0,0);
				ea_2(SIZE_W,(mrg_2 & 070)==020 ? 0 : CTR|ALT);
			}
	|	CP CPSCC cp_cond ea
			{	T_EMIT2($2 | $1 | mrg_2,0,0,0);
				T_EMIT2($3,0,0,0);
				ea_2(SIZE_B,DTA|ALT);
			}
	|	CP CPTRAPCC cp_cond SIZE imm
			{	checksize($4,2|4);
				T_EMIT2($2 | $1 | ($4>>6)+1,0,0,0);
				T_EMIT2($3,0,0,0);
				ea_2($4, 0);
			}
	|	CP TRAPCC cp_cond
			{	T_EMIT2($2 | $1 | 4,0,0,0);
				T_EMIT2($3,0,0,0);
			}
	;
cp_cond	:	'.' absexp
			{	fit(fit6($2));
				$$ = low6($2);
			}
	;
bcdx	:	ABCD
	|	ADDX sizedef
			{	$$ = $1 | $2;}
	;
creg	:	CREG
	|	SPEC	{	if ($1 != 075)
					badoperand();
				$$ = 04000;
			}
	;
off_width		/* note: these should be curly brackets, but that would
			 * leave us without brackets for expressions.
			 */
	:	'[' abs31 ':' abs31 ']'
			{	$$ = ($2<<6) | $4;
			}
	;
abs31	:	DREG	{	$$ = 040 | $1;}
	|	absexp	{	fit(fit5($1));
				$$ = low5($1);
			}
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
	|		{	mrg_2 = 0; ffew_2 = 0400; /* initialization */}
		notimmreg
	|	imm
	;
notimmreg
	:	'(' AREG ')'
			{	mrg_2 = 020 | $2;}
	|	'(' AREG ')' '+'
			{	mrg_2 = 030 | $2;}
	|	'-' '(' AREG ')'
			{	mrg_2 = 040 | $3;}
	|	'(' expr ')' sizenon
			{	bd_2 = $2; ea7071($4);
				RELOMOVE(bd_rel2, relonami);
			}
	|	'(' bd_areg_index ')'
			{	if ((mrg_2 & INDEX) == 0)
					ffew_2 |= 0100; /* suppress index */
				if (	!(mrg_2 & PC_MODE) &&
					(ffew_2 & 0300) == 0100 &&
					bd_2.typ==S_ABS && fitw(bd_2.val)
				   )
					mrg_2 = (loww(bd_2.val)?050:020) | $2;
				else {
					mrg_2 = (mrg_2&PC_MODE)?073:(060 | $2);
					ffew_2 |= 060; /* long displacement */
				}
			}
	|	'(' '[' bd_areg_index ']' index_od ')'
			{	switch(mrg_2 & INDEX) {
				case 0:
					ffew_2 |= 0163; /* suppress index */
					break;
				case DBL_INDEX:
					serror("bad indexing");
				case PRE_INDEX:
					ffew_2 |= 063; break;
				case POST_INDEX:
					ffew_2 |= 067; break;
				}
				mrg_2 = (mrg_2 & PC_MODE) ? 073 : (060 | $3);
			}
	;
imm	:	'#' expr
			{	mrg_2 = 074; bd_2 = $2;
				RELOMOVE(bd_rel2, relonami);
			}
	;
bd_areg_index
	:	/* empty */
			{	$$ = 0; ffew_2 |= 0200;
					    /* base-reg suppressed */
				bd_2.typ = S_ABS; bd_2.val = (valu_t)0;
					    /* zero displacement */
			}
	|	expr	{	$$ = 0; ffew_2 |= 0300;
				bd_2 = $1;
				RELOMOVE(bd_rel2, relonami);
			}
	|	areg_index
			{	bd_2.typ = S_ABS; bd_2.val = (valu_t)0;
			}
	|	expr ',' areg_index
			{	$$ = $3; bd_2 = $1;
				RELOMOVE(bd_rel2, relonami);
			}
	;
areg_index
	:	areg
	|	index	{	$$ = 0;
				ffew_2 |= 0200; /* base-reg suppressed */
				mrg_2 |= PRE_INDEX;
			}
	|	areg ',' index
			{	mrg_2 |= PRE_INDEX;
			}
	;
areg	:	AREG
	|	PC	{	mrg_2 |= PC_MODE;}
	|	ZPC	{	mrg_2 |= PC_MODE;
				ffew_2 |= 0200; /* base-reg suppressed */
			}
	;
index	:	reg sizedef scale
			{	checksize($2, 2|4);
				ffew_2 |= $1<<12 | ($2&0200)<<4 | $3;
			}
	;
scale	:	/* empty */
			{	$$ = 0;}
	|	'*' absexp
			{	$$ = checkscale($2);}
	;
index_od:	/* empty */
			{	od_2.typ = S_ABS; od_2.val = (valu_t)0;}
	|	',' index
			{	od_2.typ = S_ABS; od_2.val = (valu_t)0;
				mrg_2 |= POST_INDEX;
			}
	|	',' expr
			{	od_2 = $2;
				RELOMOVE(od_rel2, relonami);
			}
	|	',' index ',' expr
			{	od_2 = $4;
				mrg_2 |= POST_INDEX;
				RELOMOVE(od_rel2, relonami);
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
			{	mrg_1  = mrg_2;
				bd_1   = bd_2;
				od_1   = od_2;
				ffew_1 = ffew_2;
				RELOMOVE(bd_rel1, bd_rel2);
				RELOMOVE(od_rel1, od_rel2);
			}
		ea
	;
