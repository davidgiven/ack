#define RCSID4 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/* Author: Ed Keizer */

operation:
		BR	expr
		/* format 0 */
			{ dot_adjust(&$2) ; form0($1) ; disp(&$2, RELPC) ;}
	|	WAIT
		/* format 1 */
			{ form1($1) ;}
	|	BSR	expr
		/* format 1 */
			{ dot_adjust(&$2) ; form1($1) ; disp(&$2, RELPC) ;}
	|	RET	expr
		/* format 1 */
			{ form1($1) ; disp(&$2, 0) ;}
	|	SAVE	reg_list
		/* format 1 */
			{ form1($1) ; emit1(reg_list($2,id_op($1)!=0x6)) ;}
	|	ENTER	reg_list ',' expr
		/* format 1 */
			{ form1($1) ; emit1(reg_list($2,0)) ; disp(&$4, 0) ;}
	|	LPR	AREG ',' gen1
		/* format 2 */
			{ if ( id_op($1)==0x2 ) not_imm(&mode1) ;
			  form2($1,$2) ; gen1($1) ;
			}
	|	SEQ	gen1
		/* format 2 */
			{ form2($1,id_cc($1)) ; gen1($1) ; not_imm(&mode1) ;}
	|	MOVQ	absexp ',' gen1
		/* format 2 */
			{
			  if ( !fit4($2) ) {
				serror("Constant too large") ;
			  }
			  form2($1,low4($2)) ; gen1($1) ;
			  if ( id_op($1)!=0x1 ) not_imm(&mode1) ; /* !cmp */
			}
	|	ACB	absexp ',' gen1 ',' expr
		/* format 2 */
			{
			  dot_adjust(&$6) ;
			  if (!fit4($2) ) {
				serror("Constant too large") ;
			  }
			  form2($1,low4($2)) ; gen1($1) ; not_imm(&mode1) ;
			  disp(&$6, RELPC) ;
			}
	|	ADJSP	gen1
		/* format 3 */
			{
			  if ( id_op($1)==0 ) not_imm(&mode1) ; /* cxpd */
			  form3($1) ; gen1($1) ;}
	|	JSR	gen1
		/* format 3 */
			{
#ifndef NO_OPTIM
			  if ( mode1.m_mode==0x15 ) { /* Absolute */
				dot_adjust(&mode1.m_expr1) ;
				RELOMOVE(relonami, mode1.m_rel1);
				form1(0) ; disp(&mode1.m_expr1, RELPC) ; /* bsr */
			  } else
#endif
			  { form3($1) ; gen1($1) ; }
			  not_imm(&mode1) ;
			}
	|	JUMP	gen1
		/* format 3 */
			{
#ifndef NO_OPTIM
			  if ( mode1.m_mode==0x15 ) { /* Absolute */
				dot_adjust(&mode1.m_expr1) ;
				RELOMOVE(relonami, mode1.m_rel1);
				form0(B_TRUE) ; disp(&mode1.m_expr1, RELPC) ; /* br */
			  } else
#endif
			  { form3($1) ; gen1($1) ; }
			  not_imm(&mode1) ;
			}
	|	ADD_I	gen1 ',' gen2
		/* format 4 */
			{
			  register opc ;
			  opc=id_op($1) ;
			  if ( opc==0x9 ) not_imm(&mode1) ; /* addr */
			  if ( opc!=0x1 ) not_imm(&mode2) ; /* !cmp */
#ifndef NO_OPTIM
			  if ( mode1.m_mode==0x14 && /* Immediate */
			       (mode1.m_expr1.typ & ~S_EXT) == S_ABS &&
			       (
			         (fit4(mode1.m_expr1.val) &&
			          (opc==0 || opc==1 || opc==5))
				 ||
			         (fit4(-mode1.m_expr1.val) &&
				  (opc==8))
			       )
			     )
			   {
			       /* Warning, an absolute expression derived
				  from a symbol that is defined after
				  use might - if the value now suddenly fits -
				  cause failed assertions in newlabel
				*/
			       /* add,     cmp,     mov 	*/
			       /* added: the subtract of a signed
				* 	 short is the same as the add
				*	 of the negation of that short
				* so: subi short,x == addqi -short,x
				* 19/04/85 h.m.kodden,m.j.a.leliveld 
				*/
				if (opc==8)	/* do the negate */
					mode1.m_expr1.val = 
					(~mode1.m_expr1.val+1)&0xF;
				opc=low4(mode1.m_expr1.val) ;
				mode1= mode2 ;
				form2($1,opc) ; gen1($1) ;
			  } else
#endif
			  { form4($1) ; gengen($1) ; }
			}
	|	SETCFG	cpu_opts
		/* format 5 */
			{ form5($1,$2) ;}
	|	MOVS	string_opts
		/* format 5 */
			{ form5($1,($2)|id_cc($1)) ;}
	|	COM	gen1 ',' gen2
		/* format 6 */
			{ form6($1) ; gengen($1) ; not_imm(&mode2) ;}
	|	MUL_I	gen1 ',' gen2
		/* format 7 */
			{ 
			  if ( id_op($1)==0x9 || id_op($1)==0xB ) {
				/* mei or dei */
				switch ( mode2.m_mode ) {
				case 1 : case 3 : case 5 : case 7 :
					serror("register must be even") ;
				}
			  }
			  form7($1) ; gengen($1) ; not_imm(&mode2) ;
			}
	|	MOVID	gen1 ',' gen2
		/* format 7 */
			{ form7x($1,id_g1($1)) ; gengen($1) ; not_imm(&mode2) ;}
	|	MOVM	gen1 ',' gen2 ',' expr
		/* format 7 */
			{
			  register s_size ;
			  s_size= id_g1($1)+1 ;
			  /* $6.val= $6.val*s_size - s_size ; */
			  $6.val= $6.val -1 ;
			  form7($1) ; gengen($1) ; disp(&$6, 0) ;
			  not_imm(&mode1) ; not_imm(&mode2) ;
			}
	|	INSS	gen1 ',' gen2 ',' absexp ',' absexp
		/* format 7 */
			{ 
			  if ( ( $6<0 || $6>7 || $8<1 || $8>32 )
			     ) { serror("Constant out of bounds") ; }
			  form7($1) ; gengen($1) ;
			  if ( id_op($1)==0x3 ) not_imm(&mode1) ; /* exts */
			  not_imm(&mode2) ;
			  emit1((((int)$6)<<5)+(int)$8-1) ;
			}
	|	FFS	gen1 ',' gen2
		/* format 8 */
			{ form8($1,id_cc($1)) ; gengen($1) ; not_imm(&mode2) ;}
	|	CHECK	REG ',' gen1 ',' gen2
		/* format 8 */
			{
			  form8($1,$2) ; gengen($1) ;
			  if ( id_op($1)!=0x4 ) {
				not_imm(&mode1) ; /* check, cvtp */
				if ( id_op($1)==0x1 ) not_imm(&mode2) ;/*cvtp */
			  }
			}
	|	INS	REG ',' gen1 ',' gen2 ',' expr
		/* format 8 */
			{
			  form8($1,$2) ; gengen($1) ; disp(&$8, 0) ;
			  if ( id_op($1)==0x0 ) not_imm(&mode1) ;
			  not_imm(&mode2) ;
			}
	|	MOVIF	gen1 ',' fgen2
		/* format 9 */
			{
			  assert( id_t1($1)==T_INT && id_t2($1)==T_FL ) ;
			  form9($1,id_g1($1),id_g2($1)) ; gengen($1) ;
			  not_imm(&mode2) ;
			}
	|	MOVFL	fgen1 ',' fgen2
		/* format 9 */
			{
			  assert( id_t1($1)==T_FL && id_t2($1)==T_FL ) ;
			  form9($1,id_g1($1),( id_g2($1)==F_LONG?3:2 )) ;
			  gengen($1) ; not_imm(&mode2) ;
			}
	|	TRUNC	fgen1 ',' gen2
		/* format 9 */
			{
			  assert( id_t1($1)==T_FL && id_t2($1)==T_INT ) ;
			  form9($1,id_g2($1),id_g1($1)) ; gengen($1) ;
			  not_imm(&mode2) ;
			}
	|	LFSR	gen1
		/* format 9 */
			{ 
			  if ( id_op($1)==6 ) { /* SFSR */
				not_imm(&mode1) ;
				mode2.m_mode=mode1.m_mode ;
				mode1.m_mode=0 ;
			  } else {
				mode2.m_mode=0 ;
			  }
			  form9($1,0,0) ;
			  if ( id_op($1)==6 ) {
				mode1.m_mode=mode2.m_mode ;
			  }
			  gen1($1) ;
			}
	|	ADD_F	fgen1 ',' fgen2
		/* format 11 */
			{ if ( id_op($1)!=0x2 ) not_imm(&mode2) ; /* !CMPF */
			  form11($1) ; gengen($1) ;
			}
	|	RDVAL	gen1
		/* format 14 */
			{ form14($1,0) ; gen1($1) ; not_imm(&mode1) ;}
	|	LMR	MREG ',' gen1
		/* format 14 */
			{
			  form14($1,$2) ; gen1($1) ;
			  if ( id_op($1)==0x3 ) not_imm(&mode1) ;
			}
			/* All remaining categories are not checked for
			   illegal immediates */
	|	LCR	CREG ',' gen1
		/* format 15.0 */
			{ frm15_0($1,$2) ; gen1($1) ;}
	|	CATST	gen1
		/* format 15.0 */
			{ frm15_0($1,0) ; gen1($1) ;}
	|	LCSR	gen1
		/* format 15.1 */ /* Sure? */
			{ mode2.m_mode=0 ; frm15_1($1,0,0) ; gen1($1) ;}
	|	CCVIS	gen1 ',' gen2
		/* format 15.1 */
			{
			  assert( id_t1($1)==T_INT && id_t2($1)==T_SLAVE ) ;
			  frm15_1($1,id_g1($1),id_g2($1)) ; gengen($1) ;
			}
	|	CCVSI	gen1 ',' gen2
		/* format 15.1 */
			{ 
			  assert( id_t1($1)==T_SLAVE && id_t2($1)==T_INT ) ;
			  frm15_1($1,id_g2($1),id_g1($1)) ; gengen($1) ;
			}
	|	CCVSS	gen1 ',' gen2
		/* format 15.1 */
			{
			  assert( id_t1($1)==T_SLAVE && id_t2($1)==T_SLAVE ) ;
			  frm15_1($1,0,0) ; gengen($1) ; /* Sure? */
			}
	|	CMOV	gen1 ',' gen2
		/* format 15.5 */
			{ frm15_5($1) ; gengen($1) ;}
	;

gen1	:	{ mode_ptr= &mode1 ; clrmode() ; } gen
	;
gen2	:	{ mode_ptr= &mode2 ; clrmode() ; } gen
	;
fgen1	:	{ mode_ptr= &mode1 ; clrmode() ; } fgen
	;
fgen2	:	{ mode_ptr= &mode2 ; clrmode() ; } fgen
	;

gen	:	gen_not_reg		/* Every mode except register */
	|	REG
			{ mode_ptr->m_mode= $1 ; }
	;
fgen	:	gen_not_reg		/* Every mode except register */
	|	FREG
			{ mode_ptr->m_mode= $1 ; }
	;

gen_not_reg: 	gen_a			/* general mode with eff. address */
	|	REG { mode_ptr->m_mode=$1 ;} index
				/* The register is supposed to contain the
				   address
				*/
	|	gen_a index		/* As above, but indexed */
	|	expr			/* Immediate */
			{ mode_ptr->m_mode= 0x14 ;
			  mode_ptr->m_expr1= $1 ;
			  RELOMOVE(mode_ptr->m_rel1, relonami);
			}
	;

gen_a	:	expr '(' REG ')'		/* Register relative */
			{ mode_ptr->m_mode= 0x8 + $3 ;
			  mode_ptr->m_ndisp= 1 ;
			  mode_ptr->m_expr1= $1 ;
			  RELOMOVE(mode_ptr->m_rel1, relonami);
			}
	|	expr '(' AREG ')'		/* Memory space */
			{ if ( $3<0x8 || $3>0xA ) badsyntax() ;
			  mode_ptr->m_mode= 0x18 + ($3&3) ;
			  mode_ptr->m_ndisp= 1 ;
			  mode_ptr->m_expr1= $1 ;
			  RELOMOVE(mode_ptr->m_rel1, relonami);
			}
	|	expr '(' PC ')'			/* Memory space */
			{ mode_ptr->m_mode= 0x1B ;
			  mode_ptr->m_ndisp= 1 ;
			  mode_ptr->m_expr1= $1 ;
			  RELOMOVE(mode_ptr->m_rel1, relonami);
			  dot_adjust(&mode_ptr->m_expr1) ;
			}
	|	expr '('
			{ mode_ptr->m_expr2 = $1;
			  RELOMOVE(mode_ptr->m_rel2, relonami);
			}
	       	expr '(' AREG ')' ')'	/* Memory relative */
			{ if ( $6<0x8 || $6>0xA ) badsyntax() ;
			  mode_ptr->m_mode= 0x10 + ($6&3) ;
			  mode_ptr->m_ndisp= 2 ;
			  mode_ptr->m_expr1= $4 ;
			  RELOMOVE(mode_ptr->m_rel1, relonami);
			}
	|	'@' expr			/* Absolute */
			{ mode_ptr->m_mode= 0x15 ;
			  mode_ptr->m_ndisp= 1 ;
			  mode_ptr->m_expr1= $2 ;
			  RELOMOVE(mode_ptr->m_rel1, relonami);
			}
	|	EXTERNAL '(' expr ')' 
			{ mode_ptr->m_mode= 0x16 ;
			  mode_ptr->m_ndisp= 2 ;
			  mode_ptr->m_expr1= $3 ;
			  RELOMOVE(mode_ptr->m_rel1, relonami);
			}
		'+' expr	/* External */
			{
			  mode_ptr->m_expr2= $7 ;
			  RELOMOVE(mode_ptr->m_rel2, relonami);
			}
	|	TOS				/* Top Of Stack */
			{ mode_ptr->m_mode= 0x17 ; }
	;

index	:	'[' REG ':' INDICATOR ']'	/* Indexed postfix */
			{ mode_ptr->m_index= (mode_ptr->m_mode<<3) | $2 ;
			  mode_ptr->m_mode= ind_mode( $4 ) ;
			}
	;

cpu_opts:
		'[' ']'
			{ $$=0 ;}
	|	'[' cpu_list ']'
			{ $$= $2 ;}
	;

cpu_list:
		INDICATOR
			{ $$=cpu_opt($1) ; }
	|	cpu_list ',' INDICATOR
			{ $$= ($1) | cpu_opt($3) ;}
	;

string_opts:
			{ $$=0 ;}
	|	INDICATOR
			{ $$= string_opt($1) ; }
	|	INDICATOR ',' INDICATOR
			{ if ( $1 != 'b' ||
				( $3 != 'u' && $3 != 'w' ) ) {
					serror("illegal string options") ;
			  }
			  $$ = string_opt($1)|string_opt($3) ;
			}
	;

reg_list:
		'[' ']'
			{ $$=0 ;}
	|	'[' reg_items ']'
			{ $$= $2 ;}
	;

reg_items:
		REG
			{ $$= 1<<($1) ; }
	|	reg_items ',' REG
			{ $$= ($1) | ( 1<<($3) ) ;}
	;
