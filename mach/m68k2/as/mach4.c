/* $Header$ */
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
				Xfit(fitw($4.val));
				emit2($1 | $2);
#ifdef RELOCATION
				newrelo($4.typ, RELPC|RELO2|RELBR|RELWR);
#endif
				emit2(loww($4.val));
			}
	|	BITOP ea_ea
			{	bitop($1);}
	|	OP_EA_D sizedef ea ',' DREG
			{	if ($2 != SIZE_W) {
					serror("illegal size");
				}
				emit2($1 | mrg_2 | $5<<9);
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
						(0140610|$2|($4&07)<<9)
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
			{	Xfit(fit4($3)); emit2(047100|low4($3));}
	|	RTD imm
			{	test68010();
				emit2(047164);
				ea_2(SIZE_W, 0);
			}
	|	MODEL
			{	model = $1;}
	|	fp_op
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
fp_op	:	CP
			{	co_id = $1; }
		fp_op1
	|		{	co_id = DEF_FP; }
		fp_op1
	;
fp_op1	:	FMOVE fsize ea ',' FPCR
			{	check_fsize($2, FSIZE_L);
				if ((mrg_2&070) == 010 && $5 != 001)
					badoperand();
				emit2((0170000|co_id|mrg_2));
				emit2((0100000|($5<<10)));
				ea_2(SIZE_L, 0);
			}
	|	FMOVE fsize FPCR ',' ea
			{	check_fsize($2, FSIZE_L);
				if ((mrg_2&070) == 010 && $3 == 001)
					badoperand();
				emit2((0170000|co_id|mrg_2));
				emit2((0120000|($3<<10)));
				ea_2(SIZE_L, ALT);
			}
	|	FMOVE fsize FPREG ',' FPREG
			{	emit2(0170000|co_id);
				emit2(($3<<10)|($5<<7));
			}
	|	FMOVE fsize ea ',' FPREG
			{	ch_sz_dreg($2, mrg_2&070);
				emit2((0170000|co_id|mrg_2));
				emit2((0040000|($2<<10)|($5<<7)));
				ea_2(SIZE_L, DTA);
			}
	|	FMOVE fsize FPREG ',' ea
			{	ch_sz_dreg($2, mrg_2&070);
				if ($2 == FSIZE_P)
					serror("packed decimal needs k-factor");
				emit2((0170000|co_id|mrg_2));
				emit2((0060000|($2<<10)|($3<<7)));
				ea_2(SIZE_L, DTA|ALT);
			}
	|	FMOVE fsize FPREG ',' ea '{' '#' absexp '}'
			{	check_fsize($2, FSIZE_P);
				fit(sfit7($8));
				emit2((0170000|co_id|mrg_2));
				emit2((0066000|($3<<7)|low7($8)));
				ea_2(SIZE_L, MEM|DTA|ALT);
			}
	|	FMOVE fsize FPREG ',' ea '{' DREG '}'
			{	check_fsize($2, FSIZE_P);
				emit2((0170000|co_id|mrg_2));
				emit2((0076000|($3<<7)|($7<<4)));
				ea_2(SIZE_L, MEM|DTA|ALT);
			}
	|	FMOVECR fsize '#' absexp ',' FPREG
			{	fit(fit7($4));
				check_fsize($2, FSIZE_X);
				emit2(0170000|co_id);
				emit2(056000|($6<<7)|low7($4));
			} 
	|	FMOVEM FSIZE fregs ',' notimmreg
			{	check_fsize($2, FSIZE_X);
				if ((mrg_2&070) == 030)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2(0160000 |
					(((mrg_2&070)==040 || ($3&04000)) ?
						$3 :
						(010000|reverse($3,8))));
				ea_2(SIZE_L, MEM|ALT);
			}
	|	FMOVEM FSIZE notimmreg ',' fregs
			{	check_fsize($2, FSIZE_X);
				if ((mrg_2&070) == 040)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2((0150000|(($5&04000)?$5:reverse($5,8))));
				ea_2(SIZE_L, MEM);
			}
	|	FMOVEM SIZE fcregs ',' ea
			{	checksize($2, 4);
				if ((mrg_2&070) == 1 && $3!= 02000)
					serror("bad addressing category");
				if ((mrg_2 & 070) == 0 &&
				    $3 != 02000 && $3 != 04000 && $3 != 010000)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2((0120000|$3));
				ea_2(SIZE_L, ALT);
			}
	|	FMOVEM SIZE ea ',' fcregs
			{	checksize($2, 4);
				if ((mrg_2&070) == 1 && $5!= 02000)
					serror("bad addressing category");
				if ((mrg_2 & 070) == 0 &&
				    $5 != 02000 && $5 != 04000 && $5 != 010000)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2((0100000|$5));
				ea_2(SIZE_L, 0);
			}
	|	FDYADIC fsize ea ',' FPREG
			{	emit2((0170000|co_id|mrg_2));
				emit2((0040000|($2<<10)|($5<<7)|$1));
				ch_sz_dreg($2, mrg_2&070);
				ea_2(SIZE_L, DTA);
			}
	|	FDYADIC fsize FPREG ',' FPREG
			{	check_fsize($2, FSIZE_X);
				emit2(0170000|co_id);
				emit2(($3<<10)|($5<<7)|$1);
			}
	|	FMONADIC fsize ea ',' FPREG
			{	emit2((0170000|co_id|mrg_2));
				emit2((0040000|($2<<10)|($5<<7)|$1));
				ch_sz_dreg($2, mrg_2&070);
				ea_2(SIZE_L, DTA);
			}
	|	FMONADIC fsize FPREG ',' FPREG
			{	check_fsize($2, FSIZE_X);
				emit2(0170000|co_id);
				emit2(($3<<10)|($5<<7)|$1);
			}
	|	FMONADIC fsize FPREG
			{	check_fsize($2, FSIZE_X);
				emit2(0170000|co_id);
				emit2(($3<<10)|($3<<7)|$1);
			}
	|	FSINCOS fsize ea ',' FPREG ':' FPREG
			{	emit2(0170000|co_id|mrg_2);
				emit2(0040000|($2<<10)|($7<<7)|$1|$5);
				ea_2(SIZE_L, DTA);
			}
	|	FSINCOS fsize FPREG ',' FPREG ':' FPREG
			{	check_fsize($2, FSIZE_X);
				emit2(0170000|co_id);
				emit2(($3<<10)|($7<<7)|$1|$5);
			}
	|	FBCC expr
			{	fbranch($1, $2);}
	|	FDBCC DREG ',' expr
			{	emit2(0170110|co_id|$2);
				emit2($1);
				$4.val -= DOTVAL;
				fit(fitw($4.val));
#ifdef RELOCATION
				newrelo($4.typ, RELPC|RELO2|RELBR|RELWR);
#endif
				emit2(loww($4.val));
			}
	|	FNOP
			{	emit2(0170200|co_id);
				emit2(0);
			}
	|	FSCC ea
			{	emit2(0170100|co_id|mrg_2);
				emit2($1);
				ea_2(SIZE_B, DTA|ALT);
			}
	|	FTST fsize ea
			{	emit2((0170000|co_id|mrg_2));
				emit2((0040072|($2<<10)));
				ch_sz_dreg($2, mrg_2&070);
				ea_2(SIZE_L, DTA);
			}
	|	FTST fsize FPREG
			{	check_fsize($2, FSIZE_X);
				emit2(0170000|co_id);
				emit2(($3<<10)|072);
			}
	|	FSAVRES ea
			{	if ((mrg_2&070) == ($1&070))
					badoperand();
				emit2((0170000|co_id|($1&0700)|mrg_2));
				ea_2(0, $1&07);
			}
	|	FTRAPCC
			{	emit2(0170174|co_id);
				emit2($1);
			}
	|	FTRAPCC SIZE imm
			{	checksize($2, 2|4);
				emit2((0170170|co_id|($2==SIZE_L?03:02)));
				emit2($1);
				ea_2($2,0);
			}
	;
fregs	:	DREG
			{	$$ = 04000 | $1 << 4; }
	|	frlist
	;
frlist	:	frrange
	|	frlist '/' frrange
			{	$$ = $1 | $3;}
	;
frrange	:	FPREG
			{	$$ = 1 << $1; }
	|	FPREG '-' FPREG
			{	if ($1 > $3)
					badoperand();
				for ($$ = 0; $1 <= $3; $1++)
					$$ |= (1 << $1);
			}
	;
fcregs	:	FPCR
			{	$$ = $1 << 10; }
	|	fcregs '/' FPCR
			{	$$ = $1 | ($3 << 10); }
	;
fsize	:	/*	empty */
			{	$$ = FSIZE_X; }
	|	SIZE
			{	if ($1 == SIZE_L)
					$$ = FSIZE_L;
				else if ($1 == SIZE_W)
					$$ = FSIZE_W;
				else	$$ = FSIZE_B;
			}
	|	FSIZE
	;
