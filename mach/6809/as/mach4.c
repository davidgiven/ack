#define RCSID4 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Motorola 6809 parsing rules
 */

operation
	:	SETDP expr
			{	dpvalue = $2.val;}
	|
		NOARG
			{	emit1or2($1);}
	|
		BRANCH expr
			{	branch($1,$2);}
	|
		LBRNCH expr
			{	emit1(0x10); emit1($1);
				$2.val -= (DOTVAL+2);
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($2.typ, RELPC|RELO2|RELBR);
#endif
				emit2($2.val);
			}
	|
		SBRNCH expr
			{	emit1($1);
				$2.val -= (DOTVAL+2);
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($2.typ, RELPC|RELO2|RELBR);
#endif
				emit2($2.val);
			}
	|
		IMMED '#' expr
			{	emit1($1);
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($3.typ, RELO1);
#endif
				emit1($3.val);
			}
	|
		XOP '#' expr
			{	emit1or2($1 - 0x20);
				switch ($1 & 0x0F) {
				case 0x03:
				case 0x0C:
				case 0x0E:
#ifdef RELOCATION
					if (rflag != 0 && PASS_RELO)
						newrelo($3.typ, RELO2|RELBR);
#endif
					emit2($3.val);
					break;
				default:
#ifdef RELOCATION
					if (rflag != 0 && PASS_RELO)
						newrelo($3.typ, RELO1);
#endif
					emit1($3.val);
					break;
				}
			}
	|
		XOP '<' expr
			{	if (0 <= $1 && $1 < 0x80)
					emit1(($1-0x10) & 0x3F);
				else
					emit1or2($1 - 0x10);
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($3.typ, RELO1);
#endif
				emit1($3.val);
			}
	|
		XOP '>' expr
			{	emit1or2($1 + 0x10);
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($3.typ, RELO2|RELBR);
#endif
				emit2($3.val);
			}
	|
		STACK reglist
			{	emit1($1); emit1($2);}
	|
		TWOREG REG ',' REG
			{
				emit1($1);
				emit1($2 << 4 | $4);
			}
	|
		XOP REG
			{	switch ($2) {
				case A:	emit1($1 - 0x20);
					break;
				case B:	emit1($1 - 0x10);
					break;
				default:serror("register error");
				}
			}
	|
		XOP expr ',' REG
			{	emit1or2($1);
				offset($4,$2,0);
			}
	|
		XOP '(' expr ',' REG ')'
			{	emit1or2($1);
				offset($5,$3,0x10);
			}
	|
		XOP '(' expr ')'
			{	emit1or2($1);
				emit1(0x9F);
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($3.typ, RELO2|RELBR);
#endif
				emit2($3.val);
			}
	|
		XOP xmode
			{	emit1or2($1);
				emit1($2);
			}
	|
		XOP '(' xmode ')'
			{	if (($3 & 0x8D) == 0x80)
					serror("invalid index mode");
				emit1or2($1);
				emit1($3 + 0x10);
			}
	|
		XOP expr
			{	if (($2.typ & S_TYP) == S_ABS &&
				    ((unsigned)$2.val >> 8) == dpvalue
				) {
					if (0 <= $1 && $1 < 0x80)
						emit1(($1-0x20) & 0x3F);
					else
						emit1or2($1 - 0x10);
					emit1($2.val);
				} else {
					emit1or2($1 + 0x10);
#ifdef RELOCATION
					if (rflag != 0 && PASS_RELO)
						newrelo($2.typ, RELO2|RELBR);
#endif
					emit2($2.val);
				}
			}
	;
reglist	:	ALL
	|	REG
			{ if (($$ = regbit[$1]) < 0) serror("register error");}
	|
		reglist ',' REG
			{	register i;
				if ((i = regbit[$3]) < 0 || ($1 & i) != 0)
					serror("register error");
				$$ = $1 | i;
			}
	;
xyus	:	REG
			{ if (($$ = regno($1)) < 0) serror("register error");}
	;
xmode	:	',' xyus '+' '+'
			{	$$ = 0x81 + $2;}
	|
		',' xyus '+'
			{	$$ = 0x80 + $2;}
	|
		',' xyus
			{	$$ = 0x84 + $2;}
	|
		',' '-' '-' xyus
			{	$$ = 0x83 + $4;}
	|
		',' '-' xyus
			{	$$ = 0x82 + $3;}
	|
		REG ',' xyus
			{	switch($1) {
				case A:	$$ = 0x86 + $3; break;
				case B: $$ = 0x85 + $3; break;
				case D:	$$ = 0x8B + $3; break;
				default: serror("register error");
				}
			}
	;
