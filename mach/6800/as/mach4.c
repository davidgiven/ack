#define RCSID4 "$Header$"

/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
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
