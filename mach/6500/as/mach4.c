#define RCSID4 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Mostek 6500 parsing tables.
 */


/* From the READ_ME file - translation by Albert Koelmans:
 * ________________
 *
 * De .h en .l extensions van expressies zijn hard nodig.
 * Ze zorgen er echter ook voor dat er geen relocatable code geproduceerd
 * kan worden. Beschouw het volgende voorbeeld:
 *
 * The .h and .l extensions of expressions are vital.  However, they prevent
 * relocatable code from being generated.  Consider the following example:
 *
 * (file 1)
 * .sect .text		! 1
 * lda #[endbss].l		! 2
 * ldx #[endbss].h		! 3
 *
 * (file 2)
 * .sect .bss		! 4
 * endbss:			! 5
 *
 * Wat voor relocation struct moet er nu voor de instructie in regel 3
 * worden geproduceerd?
 * 
 * What kind of relocation structure needs to be generated for the instruction
 * in line 3?
 */

expr
	:	expr	EXTENSION
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
	:	NOPOP
			{ emit1($1); }
	|	BITOP	expr
			{ code($2,$1,$1+8); }
	|	JMPOP	expr
			{ emit1($1);
#ifdef RELOCATION
			  newrelo($2.typ, RELO2);
#endif
			  emit2($2.val);
			}
	|	JMPOP	'(' expr ')'
			{ emit1($1+0x20);
#ifdef RELOCATION
			  newrelo($3.typ, RELO2);
#endif
			  emit2($3.val);
			}
	|	JSROP	expr
			{ emit1($1);
#ifdef RELOCATION
			  newrelo($2.typ, RELO2);
#endif
			  emit2($2.val);
			}
	|	LDXOP	'#' expr
			{ emit1(0xA2);
#ifdef RELOCATION
			  newrelo($3.typ, RELO1);
#endif
			  emit1($3.val);
			}
	|	LDXOP	expr
			{ code($2,0xA6,0xAE); }
	|	LDXOP	expr ',' Y
			{ code($2,0xB6,0xBE); }
	|	LDYOP	'#' expr
			{ emit1(0xA0);
#ifdef RELOCATION
			  newrelo($3.typ, RELO1);
#endif
			  emit1($3.val);
			}
	|	LDYOP	expr
			{ code($2,0xA4,0xAC); }
	|	LDYOP	expr ',' X
			{ code($2,0xB4,0xBC); }
	|	STXOP	expr
			{ code($2,$1+0x06,$1+0x0E); }
	|	STXOP	expr ',' Y
			{ emit1($1+0x16);
#ifdef RELOCATION
			  newrelo($2.typ, RELO1);
#endif
			  emit1(lowb($2.val));
			}
	|	STYOP	expr
			{ code($2,$1+0x04,$1+0x0C); }
	|	STYOP	expr ',' X
			{ emit1($1+0x14);
#ifdef RELOCATION
			  newrelo($2.typ, RELO1);
#endif
			  emit1(lowb($2.val));
			}
	|	addop	'#' expr
			{ if ($1==0x80) serror("no store immediate");
			  emit1($1+0x09);
#ifdef RELOCATION
			  newrelo($3.typ, RELO1);
#endif
			  emit1($3.val);
			}
	|	addop	expr
			{ code($2,$1+0x05,$1+0x0D); }
	|	addop	expr ',' X
			{ code($2,$1+0x15,$1+0x1D); }
	|	addop	expr ',' Y
			{ emit1($1+0x19);
#ifdef RELOCATION
			  newrelo($2.typ, RELO2);
#endif
			  emit2($2.val);
			}
	|	addop	'(' expr ',' X ')'
			{ emit1($1+0x01);
#ifdef RELOCATION
			  newrelo($3.typ, RELO1);
#endif
			  emit1(lowb($3.val));
			}
	|	addop	'(' expr ')' ',' Y
			{ emit1($1+0x11);
#ifdef RELOCATION
			  newrelo($3.typ, RELO1);
#endif
			  emit1(lowb($3.val));
			}
	|	ROLOP	/* Default is A. */
			{ emit1($1+0x0A); }
	|	ROLOP	A
			{ emit1($1+0x0A); }
	|	ROLOP	expr
			{ code($2,$1+0x06,$1+0x0E); }
	|	ROLOP	expr ',' X
			{ code($2,$1+0x16,$1+0x1E); }
	|	BRAOP	expr
			{ branch($1,$2); }
	|	CPXOP	'#' expr
			{ emit1($1+0x00);
#ifdef RELOCATION
			  newrelo($3.typ, RELO1);
#endif
			  emit1($3.val);
			}
	|	CPXOP	expr
			{ code($2,$1+0x04,$1+0x0C); }
	|	INCOP	expr
			{ code($2,$1+0x06,$1+0x0E); }
	|	INCOP	expr ',' X
			{ code($2,$1+0x16,$1+0x1E); }
	;
addop
	:	ADDOP
	|	PSEU
			{ emit1($1>>8); $$ = $1 & 0xFF; }
	;
