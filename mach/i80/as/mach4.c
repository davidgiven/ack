#define RCSID4 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Intel 8080 parsing rules
 */
operation
	:	NOOPOP
			{	emit1($1);}
	|
		D8OP expr
			{	emit1($1);
#ifdef RELOCATION
				newrelo($2.typ, RELO1);
#endif
				emit1($2.val);
			}
	|
		D16OP expr
			{	emit1($1);
#ifdef RELOCATION
				newrelo($2.typ, RELO2);
#endif
				emit2($2.val);
			}
	|
		R16OP r16
			{	emit1($1 | ($2 << 3));}
	|
		MOV r8 ',' r8
			{	if ($2 == M && $4 == M)
					serror("register error");
				emit1($1 | ($2 << 3) | $4);
			}
	|
		MVI r8 ',' expr
			{	emit1($1 | ($2 << 3));
#ifdef RELOCATION
				newrelo($4.typ, RELO1);
#endif
				emit1($4.val);
			}
	|
		LXI r16 ',' expr
			{	emit1($1 | ($2 << 3));
#ifdef RELOCATION
				newrelo($4.typ, RELO2);
#endif
				emit2($4.val);
			}
	|
		SRCOP r8
			{	emit1($1 | $2);}
	|
		DSTOP r8
			{	emit1($1 | ($2 << 3));}
	|
		RST absexp
			{	fit(fit3($2));
				emit1($1 | (low3($2) << 3));
			}
	|
		STLDAX r16
			{
				if ($2 != B && $2 != D)
					serror("register error");
				emit1($1 | (($2 & 02) << 3));
			}
	;
r16	:	REG
			{	if ($1 & ~06)
					serror("register error");
				$$ = $1 & 06;
			}
	;
r8	:	REG
			{	if (($1 & ~07) != 0)
					serror("register error");
				$$ = $1 & 07;
			}
	;
