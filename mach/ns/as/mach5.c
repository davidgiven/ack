/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Id$"

/*
 * NS 16032 special routines
 */

void clrmode(void) {	/* clear the current mode */
	mode_ptr->m_ndisp   = 0 ;
}

int ind_mode(int type) {
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

void badsyntax(void) {

	serror("bad operands");
}

void ill_imm(void) {
	serror("immediate operand not allowed") ;
}
/* Create  the output formats */
void form2(int id, int sval) {
	assert ( id_t1(id)==T_INT ) ;
	emit1( id_g1(id) | 0xC | (id_op(id)<<4) | ((sval&1)<<7 ) ) ;
	emit1( (sval>>1) | (mode1.m_mode<<3) ) ;
}

void form3(int id) {
	assert ( id_t1(id)==T_INT ) ;
	emit1( id_g1(id) | 0x7C | ((id_op(id)&1)<<7) ) ;
	emit1( (id_op(id)>>1) | (mode1.m_mode<<3) ) ;
}

void form4(int id) {
	assert ( id_t1(id)==T_INT && id_t2(id)==T_INT );
	emit1( id_g2(id) | (id_op(id)<<2) | ((mode2.m_mode&3)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

void form5(int id, int sval) {
	assert ( id_t1(id)==T_INT ) ;
	emit1(0xE) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((sval&1)<<7 ) ) ;
	emit1( (sval>>1) ) ;
}

void form7x(int id,int i_type) {
	assert ( id_t1(id)==T_INT && id_t2(id)==T_INT );
	emit1(0xCE) ;
	emit1( i_type | (id_op(id)<<2) | ((mode2.m_mode&3)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

void form8(int id,int reg) {
	assert ( id_t1(id)==T_INT ) ;
	emit1( 0x2E | ((id_op(id)&3)<<6) ) ;
	emit1( id_g1(id) | (id_op(id)&04) | (reg<<3) | ((mode2.m_mode&03)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

void form9(int id,int i_type,int f_type) {
	emit1(0x3E) ;
	emit1( i_type | (f_type<<2) | (id_op(id)<<3) | ((mode2.m_mode&03)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

void form11(int id) {
	assert ( id_t1(id)==T_FL && id_t2(id)==T_FL && id_g1(id)==id_g2(id) );
	emit1(0xBE) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((mode2.m_mode&3)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

void form14(int id,int reg) {
	assert ( id_t1(id)==T_INT ) ;
	emit1(0x1E) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((reg&1)<<7 ) ) ;
	emit1( (reg>>1) | (mode1.m_mode<<3) ) ;
}

void frm15_0(int id,int reg) {
	assert ( id_t1(id)==T_INT ) ;
	emit1(0x16 /* + slave<<5 */ ) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((reg&1)<<7 ) ) ;
	emit1( (reg>>1) | (mode1.m_mode<<3) ) ;
}

void frm15_1(int id,int i_type,int s_type) {
	emit1(0x16 /* + slave<<5 */ ) ;
	emit1( i_type | (s_type<<2) | (id_op(id)<<3) | ((mode2.m_mode&03)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

void frm15_5(int id) {
	assert(id_t1(id)==T_SLAVE&& id_t2(id)==T_SLAVE&& id_g1(id)==id_g2(id) );
	emit1(0x16 /* + slave<<5 */ ) ;
	emit1( id_g1(id) | (id_op(id)<<2) | ((mode2.m_mode&3)<<6) ) ;
	emit1( (mode2.m_mode>>2) | (mode1.m_mode<<3) ) ;
}

void gen1(int id) {
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

void gengen(int id) {
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

void disp(register expr_t *expr, int relpc) {
	register int sm1, sm2 ;

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

void putdisp(valu_t val,int size)
{
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

void dot_adjust(register expr_t *expr)
{
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

int testsize(int type,int val)
{
/* check if value fits in type */
	switch( type ) {
	case I_DOUBLE : return fit32(val);
	case I_WORD   : return fit16(val);
	case I_BYTE   : return fit8(val);
	}
}

void imm(int i_type,register expr_t *expr)
{
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


int reg_list(int list,int reverse)
{
	register int rev_list, i ;
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

int cpu_opt(int indic)
{
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

int string_opt(int indic) {
	switch( indic ) {
	case 'b' : return SO_BACKW ;
	case 'u' : return SO_UNTIL ;
	case 'w' : return SO_WHILE ;
	default :
		serror("illegal string option %c",indic) ;
		return 0 ;
	}
}
