operation	: BRANCH optlink expr
			{branch($1, $2, $3.val);}
		| DATA1 optcond opts optp REG ',' REG ',' operand
			{data($1,$2|$3|$4|$5<<12|$7<<16,$9.val,$9.typ);}
		| DATA2 optcond opts optp REG ',' operand
			{data($1,$2|$3|$4|$5<<12,$7.val,$7.typ);}
		| DATA3 optcond opts optp REG ',' operand
			{data($1,$2|$3|$4|$5<<16,$7.val,$7.typ);}
		| SDT optcond optb optt REG ',' address
			{emit4($1|$2|$3|$4|$5<<12|$7);}
		| BDT optcond REG optexc ',' reglist optpsr
			{emit4($1|$2|$3<<16|$4|$6|$7);}
		| SWI optcond expr
			{emit4($1|$2);}
		| ADR REG ',' expr
			{calcadr($2, $4.val, $4.typ);}
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

optb		: 	{$$=0;}
		| BYTE
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
		| ',' RXX
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
			{$$ = 0x01000000|calcaddress($1.val,$1.typ,$<y_word>-1);}
		| '[' REG ']'
			{$$ = 0x01000000|$2<<16;}
		| '[' REG ',' offset ']' optexc
			{$$ = $2<<16|$4|$6|0x01000000;}
		| '[' REG ']' ',' offset
			{$$ = $2<<16|$5;}
		;

offset		: '#' expr
			{$$ = calcoffset($2.val);}
		| optsign REG optshift
			{$$ = 0x02000000|$1|$2|$3;}
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
		;
