#define RCSID4 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*	Author: Leo van Moergestel	*/

/*
 * Signetics 2650 parsing rules
 */
operation:
		NOOP
			{	emit1($1);}
	|
		ZERO ',' REG
			{	emit1($1 | $3);	}
	|
		IMMED ',' REG expr
			{	emit1($1 | $3); page();
#ifdef RELOCATION
				newrelo($4.typ, RELO1);
#endif
				emit1($4.val);
			}
	|
		REL ',' REG expr
			{	emit1($1 | $3); rela($4, 0);}
	|
		REL ',' REG '*' expr
			{	emit1($1 | $3); rela($5, 0x80);}
	|
		ZEROX ',' REG
			{	if ($3 == R0)
					serror("register error");
				emit1($1 | $3);
			}
	|
		ABSO ',' REG param
			{	emit1($1 | $3);
				absoa(par_exp, p_indexed, p_indirect);
			}
	|
		PSWM expr
			{	emit1($1); page();
#ifdef RELOCATION
				newrelo($2.typ, RELO1);
#endif
				emit1($2.val);
			}
	|
		RET ',' CC
			{	emit1($1 | $3);}
	|
		BRANCR	',' CC expr
			{	emit1($1 | $3); rela($4, 0);}
	|
		BRANCR	',' CC '*' expr
			{	emit1($1 | $3); rela($5, 0x80);}
	|
		BRANCRX	',' CC expr
			{	if($3 == 3)
					serror("wrong condition");
				emit1($1 | $3); rela($4, 0);}
	|
		BRANCRX	',' CC '*' expr
			{	if($3 == 3)
					serror("wrong condition");
				emit1($1 | $3); rela($5, 0x80);}
	|
		BRANCA ',' CC expr
			{	emit1($1 | $3); absob($4, 0);}
	|
		BRANCA ',' CC '*' expr
			{	emit1($1 | $3); absob($5, 0x8000);}
	|
		BRANCAX ',' CC expr
			{	if($3 == 3)
					serror("wrong condition");
				emit1($1 | $3); absob($4, 0);}
	|
		BRANCAX ',' CC '*' expr
			{	if($3 == 3)
					serror("wrong condition");
				emit1($1 | $3); absob($5, 0x8000);}
	|
		BRANRA ',' REG expr
			{	emit1($1 | $3); absob($4, 0);}
	|
		BRANRA ',' REG '*' expr
			{	emit1($1 | $3); absob($5, 0x8000);}
	|
		ZBRR	expr
			{	emit1($1); rela($2, 0);}
	|
		ZBRR '*' expr
			{	emit1($1); rela($3, 0x80);}
	|
		BXSA	expr
			{	emit1($1); absob($2, 0);}
	|
		BXSA '*' expr
			{	emit1($1); absob($3, 0x8000);}
	;

param:
		indir_opt
			{	p_indexed = 0;}
	|
		indir_opt ',' INDE
			{	p_indexed = 0x6000;}
	|
		indir_opt ',' '+'
			{	p_indexed = 0x2000;}
	|
		indir_opt ',' '-'
			{	p_indexed = 0x4000;}
	;

indir_opt:
		expr
			{	par_exp = $1; p_indirect = 0;}
	|
		'*' expr
			{	par_exp = $2; p_indirect = 0x8000;}
	;
