#define RCSID4 "$Header$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

operation
	:	prefix oper
	;
prefix	:	/* empty */
	|	prefix PREFIX
			{	emit1($2);}
	;
oper	:	NOOP_1
			{	emit1($1);}
	|	NOOP_2
			{	emit2($1);}
	|	JOP expr
			{	branch($1,$2);}
	|	PUSHOP ea_1
			{	pushop($1);}
	|	IOOP expr
			{	emit1($1);
#ifdef RELOCATION
				newrelo($2.typ, RELO1);
#endif
				emit1($2.val);
			}
	|	IOOP R16
			{	if ($2!=2) serror("register error");
				emit1($1+010);
			}
	|	ADDOP ea_ea
			{	addop($1);}
	|	ROLOP ea_ea
			{	rolop($1);}
	|	INCOP ea_1
			{	incop($1);}
	|	NOTOP ea_1
			{	regsize($1); emit1(0366|($1&1)); ea_1($1&070);}
	|	CALLOP ea_1
			{	callop($1&0xFFFF);}
	|	CALFOP expr ':' expr
			{	emit1($1>>8);
#ifdef RELOCATION
				newrelo($4.typ, RELO2);
#endif
				emit2($4.val);
#ifdef RELOCATION
				newrelo($2.typ, RELO2);
#endif
				emit2($2.val);
			}
	|	CALFOP mem
			{	emit1(0377); ea_2($1&0xFF);}
	|	LEAOP R16 ',' mem
			{	emit1($1); ea_2($2<<3);}
	|	ESC absexp ',' mem
			{	fit(fit6($2));
				emit1(0330 | $2>>3);
				ea_2(($2&7)<<3);
			}
	|	INT absexp
			{	if ($2==3)
					emit1(0314);
				else {
					emit1(0315); emit1($2);
				}
			}
	|	RET
			{	emit1($1);}
	|	RET expr
			{	emit1($1-1);
#ifdef RELOCATION
				newrelo($2.typ, RELO2);
#endif
				emit2($2.val);
			}
	|	XCHG ea_ea
			{	xchg($1);}
	|	TEST ea_ea
			{	test($1);}
	|	MOV ea_ea
			{	mov($1);}
mem	:	'(' expr ')'
			{	mrg_2 = 6; exp_2 = $2;
				RELOMOVE(rel_2, relonami);
			}
	|	bases
			{	exp_2.val = 0; exp_2.typ = S_ABS; indexed();}
	|	expr bases
			{	exp_2 = $1; indexed();
				RELOMOVE(rel_2, relonami);
			}
	;
bases	:	'(' R16 ')'
			{	mrg_2 = sr_m[$2];}
	|	'(' R16 ')' '(' R16 ')'
			{	mrg_2 = dr_m[$2][$5];}
	;
ea_2	:	mem
	|	R8
			{	mrg_2 = $1 | 0300;}
	|	R16
			{	mrg_2 = $1 | 0310;}
	|	RSEG
			{	mrg_2 = $1 | 020;}
	|	expr
			{	mrg_2 = 040; exp_2 = $1;
				RELOMOVE(rel_2, relonami);
			}
	;
ea_1	:	ea_2
			{	mrg_1 = mrg_2; exp_1 = exp_2;
				RELOMOVE(rel_1, rel_2);
			}
	;
ea_ea	:	ea_1 ',' ea_2
	;
