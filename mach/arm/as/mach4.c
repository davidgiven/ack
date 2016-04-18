/* $Id: mach4.c, v1.9 15-Mar-89 AJM */

operation	: BRANCH optlink expr
			{branch($1, $2, &$3);}
		| DATA1 optcond opts optp REG ',' REG ',' operand
			{data($1,$2|$3|$4|$5<<12|$7<<16,$9.val,$9.typ);}
		| DATA2 optcond opts optp REG ',' operand
			{data($1,$2|$3|$4|$5<<12,$7.val,$7.typ);}
		| DATA3 optcond opts optp REG ',' operand
			{data($1,$2|$3|$4|$5<<16,$7.val,$7.typ);}
		| SDT optcond optt REG ',' address
			{strldr($1|$2|$3|$4<<12,$6);}
		| SDT2 optcond optt REG ',' splitaddress
			{strldr($1|$2|$3|$4<<12,$6);}
		| BDT optcond REG optexc ',' reglist optpsr
			{emit4($1|$2|$3<<16|$4|$6|$7);}
		| SWI optcond expr
			{emit4($1|$2|$3.val);}
		| ADR optcond REG ',' expr
			{calcadr($2,$3,$5.val,$5.typ);}
		| MUL optcond REG ',' REG ',' REG
			{emit4($1|$2|$3<<16|$5|$7<<8);}
		| MLA optcond REG ',' REG ',' REG ',' REG
			{emit4($1|$2|$3<<16|$5|$7<<8|$9<<12);}
		;

optlink		: 	{$$=0;}
		| LINK
			{$$=$1;}
		;

optcond		: 	{$$=0xE0000000;}
		| COND
			{$$=$1;}
		;

opts		: 	{$$=0;}
		| SET
			{$$=$1;}
		;

optt		: 	{$$=0;}
		| TRANS
			{$$=$1;}
		;

optp		: 	{$$=0;}
		| PEE
			{$$=$1;}
		;

optexc		: 	{$$=0;}
		| '<'
			{$$=0x00200000;}
		;

optpsr		: 	{$$=0;}
		| '^'
			{$$=0x00400000;}
		;

operand		: REG optshift
			{$$.val = $1|$2; $$.typ = S_REG;}
		| '#'expr
			{$$ = $2;}
		;

optshift	: ',' SHIFT shftcnt
			{$$ = $2|$3;}
		| ',' RRX
			{$$ = $2;}
		|
			{$$ = 0;}
		;

aoptshift	: ',' SHIFT '#' expr
			{$$ = $2|calcshft($4.val, $4.typ, $<y_word>0);}
		| ',' RRX
			{$$ = $2;}
		|
			{$$ = 0;}
		;

shftcnt		: '#' expr
			{$$ = calcshft($2.val, $2.typ, $<y_word>0);}
		| REG
			{$$ = $1<<8|0x10;}
		;

address		: expr
			{success = 0; $$ = $1.val;}
		| '[' REG ']'
			{success = 1; $$ = 0x01000000|$2<<16;}
		| '[' REG ',' offset ']' optexc
			{success = 1; $$ = 0x01000000|$2<<16|$4|$6;}
		| '[' REG ']' ',' offset
			{success = 1; $$ = $2<<16|$5;}
		;

offset		: '#' expr
			{$$ = calcoffset($2.val);}
		| optsign REG aoptshift
			{$$ = 0x02000000|$1|$2|$3;}
		;
	
splitaddress		: expr
			{success = 0; $$ = $1.val;}
		| '[' REG ']'
			{success = 1; $$ = 0x01000000|$2<<16;}
		| '[' REG ',' splitoffset ']' optexc
			{success = 1; $$ = 0x01000000|$2<<16|$4|$6;}
		| '[' REG ']' ',' splitoffset
			{success = 1; $$ = $2<<16|$5;}
		;

splitoffset		: '#' expr
			{$$ = splitoffset($2.val);}
		;
	
optsign		:	{$$ = 0x00800000;}
		| '+'
			{$$ = 0x00800000;}
		| '-'	
			{$$ = 0x0;}
		;

reglist		: '{' rlist '}'
			{$$ = $2;}
		;

rlist		: REG
			{$$ = 1<<$1;}
		| rlist ',' REG
			{$$ = $1|1<<$3;}
		| REG '-' REG
			{
			       for ($$ = 0; $1 <= $3; $1++)
                                       $$ |= (1<<$1);
                        }
		;
