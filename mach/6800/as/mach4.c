#define RCSID4 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Motorola 6800 parsing rules
 */

operation
	:
		NOARG
			{	emit1($1);}
	|
		BRANCH expr
			{	branch($1,$2);}
	|
		XOP '#' expr
			{	emit1($1 - 0x20);
				switch ($1 & 0x0F) {
				case 0x03:
				case 0x0C:
				case 0x0E:
#ifdef RELOCATION
						newrelo($3.typ, RELBR | RELO2);
#endif
						emit2($3.val);
						break;
				default:
#ifdef RELOCATION
						newrelo($3.typ, RELO1);
#endif
						emit1($3.val);
				}
			}
	|
		XOP '<' expr
			{	emit1($1 - 0x10);
#ifdef RELOCATION
				newrelo($3.typ, RELO1);
#endif
				emit1($3.val);
			}
	|
		memref expr
			{	emit1($1 + 0x10);
#ifdef RELOCATION
				newrelo($2.typ, RELO2);
#endif
				emit2($2.val);
			}
	|
		memref expr ',' X
			{	emit1($1);
#ifdef RELOCATION
				newrelo($2.typ, RELO1);
#endif
				emit1($2.val);
			}
	;
memref	:
		AOP
	|
		XOP
	;
