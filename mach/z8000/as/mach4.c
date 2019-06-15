/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID4 "$Id$"

/*
** Zilog z8000 yacc parsing rules
*/
operation
	: f1
	| f2
	| f3
	| f4
	| f5
	| f6
	| f7
	| f8
	| f9
	;


f1	: F1_1F2_3  dst
		{ switch( ($1 & 0x0F00)>>8 ) {
		  case 9:   case 0xF: chtype( DST, TYPE_11a23 );  break;
		  case 0xC: case 0xD: chtype( DST, TYPE_11b23 );  break;
		  }
		  emit2( mode | $1 | $2<<4 );
		  if ( mode>>12 == 4 ) emit_ad( addr_inf );
		}
	| F1_1a  reg
		{ chreg( $1, $2 );
		  emit2( $1 | $2<<4 );
		}
	| F1_1b  reg  option
		{ if ( $3 != 1 && $3 != 2 )  argerr();
		  emit2( $1 | $2<<4 | ($3-1)<<1 );
		}
	| F1_2  dst  option
		{ chtype( DST, TYPE_12 );
		  fit(fit4($3-1));
		  emit2( mode | $1 | $2<<4 | $3-1 );
		  if ( mode>>12 == 4 ) emit_ad( addr_inf );
		}
	| LDK  reg  ','  imexpr
		{ fit(fit4($4));
		  emit2( $1 | $2<<4 | $4 );
		}
	| F1_2F6_3  dst  ','  src
		{ if ( oprtype[ DST ] == REG && oprtype[ SRC ] == REG )
		  {   emit2( $1 | $4 );
		      emit2( $2<<8 );
		  }
		  else if ( oprtype[ SRC ] == IM )
		  {   chtype( DST, TYPE_1263 );
		      if ((immed.typ & ~S_EXT) != S_ABS) {
			serror("must be absolute");
	 	      }
		      if ( bitset($1,8) ) /* word */  fit(fit4(immed.val));
		      else /* byte */  fit(fit3(immed.val));
		      emit2( mode | $1 | $2<<4 | (int)immed.val );
		      if ( mode>>12 == 4 ) emit_ad( addr_inf );
		  }
		  else  argerr();
		}
	| JP  coco1  dst
		{ chtype( DST, TYPE_jp );
		  emit2( mode | $1 | $3<<4 | $2 );
		  if ( mode>>12 == 4 ) emit_ad( addr_inf );
		}
	| TCC  coco1  reg
		{ emit2( $1 | $3<<4 | $2 );	}
	;


f2	: F2_1  reg  ','  src
		{ switch( ($1 & 0xF000)>>12 )
		  {   case 2: chtype( SRC, TYPE_21a );  break;
		      case 3: chtype( SRC, TYPE_21b );  break;
		  }
		  emit2( mode | $1 | $4<<4 | $2 );
		  if ( mode>>12 == 4 ) emit_ad( addr_inf );
		}
	| F2_1F5_1  dst  ','  src
		{ switch( oprtype[ DST ] )
		  { case REG: chtype( SRC, TYPE_2151 );
			      chreg( $1, $2 );
			      emit2( mode | $1 | $4<<4 | $2 );
			      break;
		    case IR: case DA: case X:
			      if ( oprtype[ SRC ] == IM
				&& ( $1 == 0x0B00 || $1 == 0x0A00   ) )
				    /* cp or cpb */
			      {   setmod( DST );
				  emit2( mode | $1 + 0x201 | $2<<4 );
				  break;
			      }
		    default: argerr();
		  }
		  if ( mode>>12 == 4 ) emit_ad( addr_inf );
		  if ( oprtype[ SRC ] == IM )
		  {   if (bitset($1,8)) /* word */ {
#ifdef RELOCATION
			newrelo(immed.typ, RELO2|RELBR);
#endif
			emit2( (int)immed.val );
		      }
		      else if (bitset($1,12)) /* long */ {
#ifdef RELOCATION
			newrelo(immed.typ, RELO4|RELWR|RELBR);
#endif
			emit4( immed.val );
		      }
		      else /* byte */ {
#ifdef RELOCATION
			newrelo(immed.typ, RELO1);
#endif
			emit1((int) immed.val);
			/* emit1((int) immed.val); ??? twice ??? */
		      }
		  }
		}
	| LDA  R32  ','  src
		{ switch( oprtype[ SRC ] )
		  {   case DA: case X: emit2( 0x7600 | $4<<4 | $2 );
				       emit_ad( addr_inf );
				       break;
		      case BA:  emit2( 0x3400 | $4<<4 | $2 );
#ifdef RELOCATION
				newrelo(displ.typ,RELO2|RELBR);
#endif
				emit2( (int) displ.val );  break;
		      case BX:  emit2( 0x7400 | $4<<4 | $2 );
				emit2( offset<<8 );  break;
		      default:  argerr();
		  }
		}
	| POP  dst  ','  ir
		{ chtype( DST, TYPE_pop );
		  emit2( mode | $1 | $4<<4 | $2 );
		  if ( mode>>12 == 4 ) emit_ad( addr_inf );
		}
	| PUSH  ir  ','  src
		{ chtype( SRC, TYPE_push );
		  switch ( oprtype[ SRC ] )
		  {   case IM: if ( $1 == 0x1100 ) /* pushl */ argerr();
			    /* {   emit2( 0x9109 | $2<<4 );
			    **	   emit4( immed );
			    ** }
			    */
			       else
			       {   emit2( 0x0D09 | $2<<4 );
#ifdef RELOCATION
				   newrelo(immed.typ, RELO2|RELBR);
#endif
				   emit2( (int)immed.val );
			       }
			       break;
		      default: emit2( mode | $1 | $2<<4 | $4 );
			       if ( mode>>12 == 4 ) emit_ad( addr_inf );
		  }
		}
	| LD  dst  ','  src
		{ if ( oprtype[ DST ] == REG )
		  {   switch( oprtype[ SRC ] )
		      {   case IM:
			       if ( $1 == 0 )  /* ldb: F3.2 */
			       {   /* fit(fits8(immed)); */
				   emit1( 0xC000 | $2<<8);
#ifdef RELOCATION
				   newrelo(immed.typ, RELO1);
#endif
				   emit1((int) immed.val);
			       }
			       else
			       {   /*fit(fits16(immed));*/
			           emit2( 0x2100 | $2 );
#ifdef RELOCATION
				   newrelo(immed.typ, RELO2|RELBR);
#endif
				   emit2( (int)immed.val );
			       }
			       break;
			  case REG: case IR: case DA: case X:
			       setmod( SRC );
			       emit2( mode | 0x2000 | $1 | $4<<4 | $2 );
			       if ( mode>>12 == 4 ) emit_ad( addr_inf );
			       break;
			  case BA: emit2( 0x3000 | $1 | $4<<4 | $2 );
#ifdef RELOCATION
				   newrelo(displ.typ,RELO2|RELBR);
#endif
				   emit2( (int) displ.val );
				   break;
			  case BX: emit2( 0x7000 | $1 | $4<<4 | $2 );
				   emit2( offset<<8 );
				   break;
			  default: argerr();
		      }
		      break;
		  }
		  if ( oprtype[ SRC ] == REG )
		  {   switch( oprtype[ DST ] )
		      {   case IR: case DA: case X:
			       setmod( DST );
			       emit2( mode | 0x2E00 | $1 | $2<<4 | $4 );
			       if ( mode>>12 == 4 ) emit_ad( addr_inf );
			       break;
			  case BA: emit2( 0x3200 | $1 | $2<<4 | $4 );
#ifdef RELOCATION
				   newrelo(displ.typ,RELO2|RELBR);
#endif
				   emit2( (int) displ.val );
				   break;
			  case BX: emit2( 0x7200 | $1 | $2<<4 | $4 );
				   emit2( offset<<8 );
				   break;
			  default: argerr();
		      }
		      break;
		  }
		  if ( oprtype[ SRC ] == IM )	/* F5.1	*/
		  {   chtype( DST, TYPE_ld );
		      emit2( mode | 0xC05 | $1 | $2<<4 );
		      if ( mode>>12 == 4 ) emit_ad( addr_inf );
		      if ( $1 == 0 ) /* ldb */
		      {   /* fit(fits8(immed)); */
#ifdef RELOCATION
			  newrelo(immed.typ, RELO1);
#endif
			  emit1((int) immed.val);
			  /* emit1((int) immed.val); ??? twice ??? */
		      }
		      else /* ld */
		      {   /*fit(fits16(immed));*/
#ifdef RELOCATION
			  newrelo(immed.typ, RELO2 | RELBR);
#endif
			  emit2( (int)immed.val );
		      }
		      break;
		  }
		  argerr();
		}
	| LDL  dst  ','  src
		{ if ( oprtype[ DST ] == REG )
		  {   switch( oprtype[ SRC ] )
		      {   case IM: emit2( 0x1400 | $2 );
#ifdef RELOCATION
			  	   newrelo(immed.typ, RELO4|RELBR|RELWR);
#endif
				   emit4( immed.val );
				   break;
			  case REG: case IR: case DA: case X:
			       setmod( SRC );
			       emit2( mode | 0x1400 | $4<<4 | $2 );
			       if ( mode>>12 == 4 ) emit_ad( addr_inf );
			       break;
			  case BA: emit2( 0x3500 | $4<<4 | $2 );
#ifdef RELOCATION
				   newrelo(displ.typ,RELO2|RELBR);
#endif
				   emit2((int) displ.val );
				   break;
			  case BX: emit2( 0x7500 | $4<<4 | $2 );
				   emit2( offset<<8 );
				   break;
			  default: argerr();
		      }
		      break;
		  }
		  if ( oprtype[ SRC ] == REG )
		  {   switch( oprtype[ DST ] )
		      {   case IR: case DA: case X:
			       setmod( DST );
			       emit2( mode | 0x1D00 | $2<<4 | $4 );
			       if ( mode>>12 == 4 ) emit_ad( addr_inf );
			       break;
			  case BA: emit2( 0x3700 | $2<<4 | $4 );
#ifdef RELOCATION
				   newrelo(displ.typ,RELO2|RELBR);
#endif
				   emit2( (int) displ.val );
				   break;
			  case BX: emit2( 0x7700 | $2<<4 | $4 );
				   emit2( offset<<8 );
				   break;
			  default: argerr();
		      }
		      break;
		  }
	    /*	  if ( oprtype[ SRC ] == IM )
	    **	  {   chtype( DST, TYPE_ld );
	    **	      emit2( mode | 0xD07 | $2<<4 );
	    **	      if ( mode>>12 == 4 ) emit_ad( addr_inf );
	    **	      emit4( immed );
	    **	      break;
	    **	  }
	    */
		  argerr();
		}
	;


f3	: DJNZ  reg  ','  ra
		{ branch( $1 | $2<<8, $4 );	}
	| JR  coco1  ra
		{ branch( $1 | $2<<8, $3 );	}
	| CALR  ra
		{ branch( $1, $2 );	}
	;


f4	: LDR  reg  ','  ra
		{ ldrel( $1 | $2, $4 );		}
	| LDR  ra  ','  reg
		{ ldrel( $1 | 0x200 | $4, $2 );	}
	| LDAR  R32  ','  ra
		{ ldrel( $1 | $2, $4 );		}
	;


f5	: F5_1L  reg  option
		{   if ( $3 < 0 )
		    {	warning( "neg src results in a right shift!" );
			warning( "warning only");
		    }
		    shiftcode( $1 | $2<<4, $3 );
		}
	| F5_1R  reg  option2
		{   if ( $3 > 0 )
		    {	warning( "pos src results in a left shift!" );
			warning( "warning only");
		    }
		    shiftcode( $1 | $2<<4, $3 );
		}
	;

option2	: ',' imexpr
		{ $$ = $2;	}
	|  /* empty */
		{ $$ = -1;	}
	;

f6	: LDM  dst  ','  src  ','  imexpr
		{ switch( oprtype[ DST ] )
		  { case REG: chtype( SRC, TYPE_ldm );
			      ldmcode( $1 | $4<<4, $2, $6 );
			      break;
		    default:  switch( oprtype[ SRC ] )
			      { case REG: chtype( DST, TYPE_ldm );
					  ldmcode($1+8 | $2<<4, $4, $6);
					  break;
				default:  argerr();
			      }
		  }
		}
	| F6_4  ir  ','  ir  ','  R16
		{ /* For translate instructions the roles of $2 and $4
		  ** are interchanged with respect to the other
		  ** instructions of this group.
		  */
		  if ( ($1 & 0xB8FF) == $1 )
		  {	/* translate instruction	*/
			emit2( ($1 & ~0xF0) | $2<<4 );
			emit2( ($1 & 0xF0)>>4 | $6<<8 | $4<<4 );
		  }
		  else
		  {	emit2( ($1 & ~0xF0) | $4<<4 );
			emit2( ($1 & 0xF0)>>4 | $6<<8 | $2<<4 );
		  }
		}
	| F6_5  dst  ','  ir  ','  R16  coco2
		{ switch( oprtype[ DST ] )
		  {   case REG: if ( bitset($1,1) )  argerr();  break;
		      case IR : if ( !bitset($1,1) )  argerr();  break;
		      default : argerr();
		  }
		  emit2( $1 | $4<<4 );
		  emit2( $6<<8 | $2<<4 | $7 );
		}
	| F6_6  reg  ','  R16
		{ emit2( $1 | $2<<4 );
		  emit2( $4<<8 );
		}
	;


f7	: IN  reg  ','  da
		{ emit2( $1 | 0xA04 | $2<<4 );
#ifdef RELOCATION
		  newrelo(adr_inf.typ, RELO2|RELBR);
#endif
		  emit2( (short)addr_inf.val );    /* i/o address */
		}
	| OUT  da  ','  reg
		{ emit2( $1 | 0xA06 | $4<<4 );
#ifdef RELOCATION
		  newrelo(adr_inf.typ, RELO2|RELBR);
#endif
		  emit2( (short)addr_inf.val );    /* i/o address */
		}
	| IN  reg  ','  ir
		{ if ( bitset($1,0) )	argerr();
		  emit2( $1 | 0xC00 | $4<<4 | $2 );
		}
	| OUT  ir  ','  reg
		{ if ( bitset($1,0) )	argerr();
		  emit2( $1 | 0xE00 | $2<<4 | $4 );
		}
	;


f8	: LDCTL  ctlargs
		{ emit2( $1 | $2 );	}
	| LDCTLB  ctlbargs
		{ emit2( $1 | $2 );	}
	| MREQ  reg
		{ emit2( $1 | $2<<4 );	}
	;
ctlargs	: CTLR  ','  R16
		{ $$ = $3<<4 | $1 | 8;	}
	| R16  ','  CTLR
		{ $$ = $1<<4 | $3;	}
	;
ctlbargs: CTLRFLAGS  ','  R8
		{ $$ = $3<<4 | $1 | 8;}
	| R8  ','  CTLRFLAGS
		{ $$ = $1<<4 | $3;	}
	;




f9	: F9_1  flags
		{ emit2( $1 | $2 );	}
	| F9_2  ints
		{ emit2( $1 | $2 );	}
	| F9_3
		{ emit2( $1 );	}
	| RET
		{ emit2( $1 | 8 );	}
	| RET  CC
		{ emit2( $1 | $2 );	}
	| SC  imexpr
		{ fit(fit8($2));
		  emit2( $1 | $2 );
		}
	;
flags	: flags  ','  FLAG
		{ $$ = $1 | $3;	}
	| FLAG
		{ $$ = $1;	}
	;
ints	: ints  ','  INTCB
		{ $$ = $1 | $3;	}
	| INTCB
		{ $$ = $1;	}
	;




coco1	: CC  ','
		{ $$ = $1;	}
	| /* empty */
		{ $$ = 8;	}
	;
coco2	: ','  CC
		{ $$ = $2;	}
	| /* empty */
		{ $$ = 8;	}
	;
option	: ','  imexpr
		{ $$ = $2;	}
	|  /* empty */
		{ $$ = 1;	}
	;
/*    `imexpr', just as `im', is used to implement immediate data.
** But `imexpr' is used in those cases where the  immediate  value
** always  will fit into 16 bits, so (long) `immed' is not needed.
** Those cases are in `option', `option2', f9-`SC',  f6-`LDM'  and
** f1-`LDK'.
*/
imexpr	: '$'  absexp
		{ $$ = $2;	}
	;
/*     Destination (dst) as well as source  (src)  operands  never
** have RA as addressing mode, except for some instructions of the
** F3 and F4 instruction format group.   In those cases RA is even
** the  only  addressing mode which is allowed.   This is why `ra'
** has a yacc-rule not being part of `opr'.
*/
ra	: expr
		{ $$ = $1;	}
	;
dst	:	{ operand = DST;}
	  opr
		{ $$ = $2;	}
	;
src	:	{ operand = SRC;}
	  opr
		{ $$ = $2;	}
	;
opr	: reg
		{ settype( REG );	}
	| im
		{ settype( IM );	}
	| ir
		{ settype( IR );	}
	| da
		{ settype( DA );	}
	| x
		{ settype( X );		}
	| ba
		{ settype( BA );	}
	| bx
		{ settype( BX );	}
	;
reg	: R8
	| R16
	| R32
	| R64
	;
im	: '$'  expr
		{ $$ = 0;
		  immed = $2;
		}
	| '$' '<' '<' expr '>' '>' expr
		{ $$ = 0;
		  immed.typ = combine($4.typ, $7.typ, '+');
		  immed.val = $4.val<<16 | $7.val;
		}
	;
ir	: '*'  R32
		{ if ( $2 == 0 ) regerr();
		  $$ = $2;
		}
	;
da	: expr
		{ $$ = 0;
		  addr_inf = $1;
		}
	| '<' '<' expr '>' '>' expr
		{ $$ = 0;
		  addr_inf.typ = combine( $3.typ, $6.typ, '+' );
		  addr_inf.val = $3.val<<16 | $6.val;
		}
	;
x	: expr  '('  R16  ')'
		{ if ( $3 == 0 ) regerr();
		  $$ = $3;
		  addr_inf = $1;
		}
	| '<' '<'  expr  '>' '>'  expr  '('  R16  ')'
		{ if ( $8 == 0 ) regerr();
		  $$ = $8;
		  addr_inf.typ = combine( $3.typ, $6.typ, '+' );
		  addr_inf.val = $3.val<<16 | $6.val;
		}
	;
ba	: R32  '('  '$'  expr  ')'
		{ if ( $1 == 0 ) regerr();
		  $$ = $1;
		  displ = $4;
		}
	;
bx	: R32  '('  R16  ')'
		{ if ( $1 == 0 || $3 == 0 ) regerr();
		  $$ = $1;
		  offset = $3;
		}
	;
