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
 * Mostek 6500 parsing tables.
 */

expr
	:	expr	EXTENSION
			{ $$.val = ($1.val >> $2) & 0xFF;
			  $$.typ = combine($1.typ, S_ABS, '&');
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
			  emit1($3);
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
			  emit1($3);
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
			  emit1($3);
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
			  emit1($3);
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
