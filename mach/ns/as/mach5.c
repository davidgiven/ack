/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Id$"

/*
 * NS 16032 special routines
 */

clrmode() {	/* clear the current mode */
	mode_ptr->m_ndisp   = 0 ;
}

int ind_mode(type) {
	switch ( type ) {
	case 'b' :	return 0x1C ;
	case 'w' :	return 0x1D ;
	case 'd' :	return 0x1E ;
	case 'q' :	return 0x1F ;
	default :
			serror("illegal size indicator") ;
			return 0x1F ;
	}
}

badsyntax() {

	serror("bad operands");
}

ill_imm() {
	serror("immediate operand not allowed") ;
}
/* Create  the output formats */
form2(id,sval) {
	assert ( id_t1(id)==T_INT ) ;
	emit1( id_g1(id) | 0xC | (id_op(id)<<4) | ((sval&1)<<7 ) ) ;
	emit1( (sval>>1) | (mode1.m_mode<<3) ) ;
}

form3(id) {
	assert ( id_t1(id)==T_INT ) ;
	emit1( id_g1(id) | 0x7C | ((id_op(id)&1)<<7) ) ;
	emit1( (id_op(id)>>1) | (mode1.m_mode<<3) ) ;
}

form4(id) {
	assert ( id_t1(id)==T_INT && id_t2(id)==T_INT );
	emit1( id_g2(id) | (id_op(id)<<2) | ((mode2.m_mode&3)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

form5(id,sval) {
	assert ( id_t1(id)==T_INT ) ;
	emit1(0xE) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((sval&1)<<7 ) ) ;
	emit1( (sval>>1) ) ;
}

form7x(id,i_type) {
	assert ( id_t1(id)==T_INT && id_t2(id)==T_INT );
	emit1(0xCE) ;
	emit1( i_type | (id_op(id)<<2) | ((mode2.m_mode&3)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

form8(id,reg) {
	assert ( id_t1(id)==T_INT ) ;
	emit1( 0x2E | ((id_op(id)&3)<<6) ) ;
	emit1( id_g1(id) | (id_op(id)&04) | (reg<<3) | ((mode2.m_mode&03)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

form9(id,i_type,f_type) {
	emit1(0x3E) ;
	emit1( i_type | (f_type<<2) | (id_op(id)<<3) | ((mode2.m_mode&03)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

form11(id) {
	assert ( id_t1(id)==T_FL && id_t2(id)==T_FL && id_g1(id)==id_g2(id) );
	emit1(0xBE) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((mode2.m_mode&3)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

form14(id,reg) {
	assert ( id_t1(id)==T_INT ) ;
	emit1(0x1E) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((reg&1)<<7 ) ) ;
	emit1( (reg>>1) | (mode1.m_mode<<3) ) ;
}

frm15_0(id,reg) {
	assert ( id_t1(id)==T_INT ) ;
	emit1(0x16 /* + slave<<5 */ ) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((reg&1)<<7 ) ) ;
	emit1( (reg>>1) | (mode1.m_mode<<3) ) ;
}

frm15_1(id,i_type,s_type) {
	emit1(0x16 /* + slave<<5 */ ) ;
	emit1( i_type | (s_type<<2) | (id_op(id)<<3) | ((mode2.m_mode&03)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

frm15_5(id) {
	assert(id_t1(id)==T_SLAVE&& id_t2(id)==T_SLAVE&& id_g1(id)==id_g2(id) );
	emit1(0x16 /* + slave<<5 */ ) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((mode2.m_mode&3)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

gen1(id) {
	if ( (mode1.m_mode&0x1C)==0x1C ) {
		emit1(mode1.m_index) ;
	}
	if ( mode1.m_mode==0x14 ) { /* Immediate */
		RELOMOVE(relonami, mode1.m_rel1);
		imm(id_g1(id),&mode1.m_expr1) ;
	} else
	if ( mode1.m_ndisp >0 ) {
		RELOMOVE(relonami, mode1.m_rel1);
		disp(&mode1.m_expr1, mode1.m_mode == 0x1B ? RELPC : 0) ;
		if ( mode1.m_ndisp >1 ) {
			RELOMOVE(relonami, mode1.m_rel2);
			disp(&mode1.m_expr2, 0) ;
		}
	}
}

gengen(id) {
	if ( (mode1.m_mode&0x1C)==0x1C ) {
		emit1(mode1.m_index) ;
	}
	if ( (mode2.m_mode&0x1C)==0x1C ) {
		emit1(mode2.m_index) ;
	}
	if ( mode1.m_mode==0x14 ) { /* Immediate */
		RELOMOVE(relonami, mode1.m_rel1);
		imm(id_g1(id),&mode1.m_expr1) ;
	} else
	if ( mode1.m_ndisp >0 ) {
		RELOMOVE(relonami, mode1.m_rel1);
		disp(&mode1.m_expr1, mode1.m_mode == 0x1B ? RELPC : 0) ;
		if ( mode1.m_ndisp >1 ) {
			RELOMOVE(relonami, mode1.m_rel2);
			disp(&mode1.m_expr2, 0) ;
		}
	}
	if ( mode2.m_mode==0x14 ) { /* Immediate */
		RELOMOVE(relonami, mode2.m_rel1);
		imm(id_g2(id),&mode2.m_expr1) ;
	} else
	if ( mode2.m_ndisp >0 ) {
		RELOMOVE(relonami, mode2.m_rel1);
		disp(&mode2.m_expr1, mode2.m_mode == 0x1B ? RELPC : 0) ;
		if ( mode2.m_ndisp >1 ) {
			RELOMOVE(relonami, mode2.m_rel2);
			disp(&mode2.m_expr2, 0) ;
		}
	}
}

disp(expr, relpc) register expr_t *expr ; {
	register sm1, sm2 ;

	sm1=0 ; sm2=0 ;
	if (DOTTYP >= 0x2 && DOTTYP<=0x7F &&
#ifdef ASLD
	    (pass==PASS_1 ? expr->typ==S_ABS : expr->typ!=S_VAR ) ) {
#else
	   expr->typ == S_ABS) {
#endif
		/* All non-text displacements are quad-sized.
		   The algorithm currently used for minimizing the size
		   (See CACM article) might generate assertion failures if
		   any displacement it is trying to minimize increases during
		   the assembly process. The only way to avoid increases
		   like that is to fix the sizes of all displacements in
		   non-text segments.
		   If the framework included enough information one
		   might detect in the first pass (0) whether the value
		   of the expression depends on any symbols whose value
		   might alter later on in the assembly process. In that case
		   one could determine the right size in the first pass in
		   most cases.
		*/
		if ( fitd_b(expr->val) ) {
			sm1=1 ; sm2= 1 ;
		} else {
			if ( fitd_w(expr->val) ) sm2=1 ;
		}
	}
	sm1=small(sm1,1) ; sm2=small(sm2,2) ;
#ifdef RELOCATION
	newrelo(expr->typ, RELO4|RELBR|RELWR|relpc);
#endif
	if ( sm1 ) 	putdisp(expr->val,1) ;
	else if ( sm2 ) putdisp(expr->val,2) ;
	else		putdisp(expr->val,4) ;
}

putdisp(val,size) valu_t val ; {
	switch ( size ) {
	case 1 :
		emit1( ((int)val)&0x7F ) ;
		break ;
	case 2 :
		emit1( ( (((int)val)>>8)&0x3F ) | 0x80 ) ;
		emit1( ((int)val)&0xFF ) ;
		break ;
	case 4 :
		emit1( (((int)(val>>24)) | 0xC0) & 0xFF ) ;
		emit1( ((int)(val>>16)) & 0xFF ) ;
		emit1( (((int)val)>>8) & 0xFF ) ;
		emit1( ((int)val)&0xFF ) ;
		break ;
	}
}

dot_adjust(expr) register expr_t *expr ; {
	expr->val -= DOTVAL ;
	if ( pass==PASS_2 ) {
		if ( (expr->typ&S_DOT) == 0 && expr->val>0 ) {
			expr->val -= DOTGAIN;
		}
	}
	if ( (expr->typ & ~S_DOT) == DOTTYP ) {
		expr->typ=S_ABS ;
	} else {
		expr->typ=S_VAR ;
	}
}

/* The idea of the following is:
 * Give the assembler programmer a warning if he tries to store
 * an immediate value in a field which is too small to fit in.
 */

testsize(type,val) {
/* check if value fits in type */
	switch( type ) {
	case I_DOUBLE : return fit32(val);
	case I_WORD   : return fit16(val);
	case I_BYTE   : return fit8(val);
	}
}

imm(i_type,expr) register expr_t *expr ; {
/* emit value of immediate expression , after check on FIT */
	if (!testsize(i_type,(int)expr->val))
		warning("immediate operand too large");
	switch( i_type ) {
	case I_DOUBLE : 
#ifdef RELOCATION
			newrelo(expr->typ, RELO4|RELBR|RELWR);
#endif
			emit1( ((int)(expr->val>>24)) & 0xFF ) ;
			emit1( ((int)(expr->val>>16)) & 0xFF ) ;
			emit1( (((int)expr->val)>>8)  & 0xFF ) ;
			emit1(  ((int)expr->val)      & 0xFF ) ;
			break;
	case I_WORD:
#ifdef RELOCATION
			newrelo(expr->typ, RELO2|RELBR);
#endif
			emit1( (((int)expr->val)>>8)  & 0xFF ) ;
			emit1(  ((int)expr->val)      & 0xFF ) ;
			break;
	case I_BYTE:	
#ifdef RELOCATION
			newrelo(expr->typ, RELO1);
#endif
			emit1(  ((int)expr->val)      & 0xFF ) ;
	}
}


reg_list(list,reverse) {
	register rev_list, i ;
	if ( !reverse ) {
		return list ;
	}
	rev_list= 0 ;
	for ( i=0 ; i<8 ; i++ ) {
		if ( list & (1<<i) ) {
			rev_list |= 1<<(7-i) ;
		}
	}
	return rev_list ;
}
cpu_opt(indic) {
	switch( indic ) {
	case 'i' : return 1 ;
	case 'f' : return 2 ;
	case 'm' : return 4 ;
	case 'c' : return 8 ;
	default :
		serror("illegal cpu option %c",indic) ;
		return 0 ;
	}
}

string_opt(indic) {
	switch( indic ) {
	case 'b' : return SO_BACKW ;
	case 'u' : return SO_UNTIL ;
	case 'w' : return SO_WHILE ;
	default :
		serror("illegal string option %c",indic) ;
		return 0 ;
	}
}
