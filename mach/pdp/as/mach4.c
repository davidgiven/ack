#define RCSID4 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * PDP 11 dependent yacc table
 */

operation
	: OP_SO opr1
		{ emit2( $1 | $2 ); op1($2); }
	| OP_DO opr1 ',' opr2
		{ emit2( $1 | ($2 << 6) | $4); op1($2); op2($4);}
	| OP_R_SO REG ',' opr1
		{ emit2( $1 | ($2 << 6) | $4); op1($4); }
	| BR expr
		{ branch( $1, $2); }
	| EJMP expr
		{ ejump( $1, $2); }
	| JMP opr1
		{ jump( $1, $2); }
	| JSR REG ',' opr1
		{ emit2( $1 | ($2 << 6) | $4); op1($4); }
	| RTS REG
		{ emit2($1 | $2); }
	| MARK absexp
		{ fit(fit6($2)); emit2($1 | $2); }
	| SOB REG ',' expr
		{ sob($2, $4); }
	| SPL absexp
		{ fit(fit3($2)); emit2( $1 | $2 ); }
	| TRAP absexp
		{ fit(fit8($2)); emit2( $1 | $2 ); }
	| OP_NO
		{emit2($1); }
	| clcc
		{ emit2($1); }
	| secc
		{ emit2($1); }
	| OP_SO_R opr1 ',' REG
		{ emit2( $1 | ($4 << 6) | $2); op1($2); }
	| MFP opr1
		{ emit2( $1 | $2); op1($2); }
	| FOP_FSO_FR fopr1 ',' fregister
		{emit2($1 | ($4 << 6) | $2); op1($2); }
	| FOP_FSO fopr1
		{ emit2( $1 | $2); op1($2); }
	| FOP_SO opr1
		{ emit2( $1 | $2); op1($2); }
	| MOVF fop2 ',' fregister
		{ emit2(OPLDF | ($4 << 6) | $2); op2($2);}
	| MOVF fregister ',' fopr1
		{ emit2(OPSTF | ($2 << 6) | $4); op1($4); }
	| FOP_SO_FR opr1 ',' fregister
		{ emit2($1 | ($4 << 6) | $2); op1($2); }
	| FOP_FR_SO fregister ',' opr1
		{ emit2($1 | ($2 << 6) | $4); op1($4); }
	| FOP_FR_FSO fregister ',' fopr1
		{ emit2($1 | ($2 << 6) | $4); op1($4); }
	;
clcc
	: CLEARCC
	| clcc '|' CLEARCC
		{ $$ = ( $1 | $3 ); }
	;
secc
	: SETCC
	| secc '|' SETCC
		{ $$ = ( $1 | $3 ); }
	;
opr1
	: opr2
		{ $$ = $1;
		  exp_1 = exp_2;
		  RELOMOVE(rel_1, rel_2);
		  im1flag = im2flag;
		  im2flag=0;
		}
	;
opr2
	: REG
	| op2
	;
op2
	: ari
		{ $$ = ( RDEFMODE | $1); }
	| '*' REG
		{ $$ = ( RDEFMODE | $2); }
	| ainc
		{ $$ = ( AIMODE | $1 ); }
	| adec
		{ $$ = ( ADMODE | $1 ); }
	| expr ari
		{ exp_2 = $1; $$ = ( INDMODE | $2); im2flag = 1;
		  RELOMOVE(rel_2, relonami);
		}
	| '*' expr ari
		{ exp_2 = $2; $$ = ( INDDEFMODE | $3); im2flag = 1;
		  RELOMOVE(rel_2, relonami);
		}
	| '*' ainc
		{ $$ = ( AIDEFMODE | $2 ); }
	| '*' adec
		{ $$ = ( ADDEFMODE | $2 ); }
	| expr
		{ exp_2 = $1; $$ = (INDMODE | 07); im2flag = 1; 
		  RELOMOVE(rel_2, relonami);
		}
	| '$' expr
		{ exp_2 =$2; $$ = ( AIMODE | 07); im2flag = 1; 
		  RELOMOVE(rel_2, relonami);
		}
	| '*' '$' expr
		{ exp_2 = $3; $$ = ( AIDEFMODE | 07 ); im2flag = 1; 
		  RELOMOVE(rel_2, relonami);
		}
	| '*' expr
		{ exp_2 = $2; $$ = ( INDDEFMODE | 07 ); im2flag = 1; 
		  RELOMOVE(rel_2, relonami);
		}
	;
fopr1
	: fopr2
		{ $$ = $1; exp_1 = exp_2; im1flag = im2flag; im2flag=0;
		  RELOMOVE(rel_1, rel_2);
		}
	;
fopr2
	: fregister
	| fop2
	;
fop2
	: FRSP
	| op2
	;
ari
	: '(' REG ')'
		{ $$ = $2; }
	;
ainc
	: ari '+'
	;
adec
	: '-' ari
		{ $$ = $2; }
	;
fregister
	: FREG
	| REG
	;
