#define RCSID4 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Motorola 6805 parsing rules
 */

expr8
	:
		expr
			{
				fit(fitb($1.val));
			}
	;
bitexp
	:
		absexp
			{
				fit((unsigned) $1 <= 7);
				$$ = $1 & 07;
			}
	;
operation
	:
		NOARG
			{	emit1($1);}
	|
		BRANCH expr
			{	branch($1,$2);}
	|
		BBRANCH bitexp ',' expr8 ',' expr
			{
				branch($1+($2 << 1), $6, $4);
			}
	|
		BIT bitexp ',' expr8
			{
				emit1($1+($2 << 1));
#ifdef RELOCATION
				newrelo($4.typ, RELO1);
#endif
				emit1($4.val);
			}
	|
		RMR '<' expr8
	|
		RMR expr8
			{
				emit1($1);
#ifdef RELOCATION
				newrelo($2.typ, RELO1);
#endif
				emit1($2.val);
			}
	|
		RMR expr8 ',' X
			{
				if(small($2.val == 0 &&
					 ($2.typ & ~S_DOT) == DOTTYP &&
					 pass == PASS_2, 1))
					emit1($1+0x40);
				else
				{
					emit1($1+0x30);
#ifdef RELOCATION
					newrelo($2.typ, RELO1);
#endif
					emit1($2.val);
				}
			}
	|
		RMR ',' X
			{	emit1($1+0x40); }
	|
		RM '#' expr8
			{
				switch($1) {
					case 0xa7:
					case 0xac:
					case 0xad:
					case 0xaf:
						serror("mode error");
					default:
						emit1($1);
				}
#ifdef RELOCATION
				newrelo($3.typ, RELO1);
#endif
				emit1($3.val);
			}
	|
		RM '<' expr8
			{
				emit1($1+0x10);
#ifdef RELOCATION
				newrelo($3.typ, RELO1);
#endif
				emit1($3.val);
			}
	|
		RM expr
			{
				if(small(pass == PASS_2 &&
					 ($2.typ & ~S_DOT) == DOTTYP &&
					 fitb($2.val),1)) {
					emit1($1+0x10);
#ifdef RELOCATION
					newrelo($2.typ, RELO1);
#endif
					emit1($2.val);
				} else {
					emit1($1+0x20);
#ifdef RELOCATION
					newrelo($2.typ, RELO2|RELBR);
#endif
					emit2($2.val);
				}
			}
	|
		RM '>' expr
			{
				emit1($1+0x20);
#ifdef RELOCATION
				newrelo($3.typ, RELO2|RELBR);
#endif
				emit2($3.val);
			}
	|
		RM '>' expr ',' X
			{
				emit1($1+0x30);
#ifdef RELOCATION
				newrelo($3.typ, RELO2|RELBR);
#endif
				emit2($3.val);
			}
	|
		RM expr ',' X
			{
				if(small(pass == PASS_2 &&
					 ($2.typ & ~S_DOT) == DOTTYP &&
					 fitb($2.val),1)) {
					if(small(pass == PASS_2 && 
					 	 ($2.typ & ~S_DOT) == DOTTYP &&
						 $2.val == 0,1))
						emit1($1+0x50);
					else {
						emit1($1+0x40);
#ifdef RELOCATION
						newrelo($2.typ, RELO1);
#endif
						emit1($2.val);
					}
				} else {
					small(0,1);  /* dummy */
					emit1($1+0x30);
#ifdef RELOCATION
					newrelo($2.typ, RELO2|RELBR);
#endif
					emit2($2.val);
				}
			}
	|
		RM '<' expr8 ',' X
			{
				emit1($1+0x40);
#ifdef RELOCATION
				newrelo($3.typ, RELO1);
#endif
				emit1($3.val);
			}
	|
		RM ',' X
			{	emit1($1+0x50); }
	|
		CMOS
			{
				switch($1) {
					case 0:
						opt_cmos = 1;
						break;
					case 0x8e:
					case 0x8f:
						if(! opt_cmos)
						serror("bad opcode");
						emit1($1);
				}
			}
	;
