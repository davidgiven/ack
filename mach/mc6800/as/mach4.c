#define RCSID4 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Motorola 6800 parsing rules
 */

expr    
        :       expr    EXTENSION       
                        { $$.val = ($1.val >> $2) & 0xFF;
#ifdef RELOCATION       
                          $$.typ = combine($1.typ, S_ABS, '&');
                                            /* This will generate an 'invalid operator'      */         
                                            /* error if $1.typ is not absolute after pass 1. */         
#else   
                          $$.typ = $1.typ;  /* Even if $1.typ is relocatable, it should be   */         
                                            /* absolute by the final pass.                   */
#endif /* RELOCATION */
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
		XOP '#' expr
			{	emit1($1 - 0x20);
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
				}
			}
	|
		XOP '<' expr
			{	emit1($1 - 0x10);
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($3.typ, RELO1);
#endif
				emit1($3.val);
			}
	|
		memref expr
			{	emit1($1 + 0x10);
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($2.typ, RELO2|RELBR);
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
