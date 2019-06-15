/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Id$"

void settype( int type )
{	oprtype[ operand ] = type;	}


int	twolog(int s)
{	int	twopower = 0;
	while ( (s>>=1) != 0 )  twopower++;
	return( twopower );
}


void setmod(int opr)
{
    mode = modetbl[ twolog( oprtype[opr] ) ] << 12;
}


void chtype(int opr, int typerange )
/* Check type of 'opr' with given 'typerange' and
** set the global var 'mode'.
*/
{	if ( (oprtype[opr] & typerange) != oprtype[opr] )  argerr();
	else /* We have a permitted type for 'opr'. */  setmod( opr );
}


void chreg(int opc, int reg)
{	switch( opc ) {
	case 0xB10A: case 0x1B00: case 0x1900:
		/* R32 expected	*/  if (reg & 1) regerr();  break;
	case 0xB107: case 0x1A00: case 0x1800:
		/* R64 expected	*/  if (reg & 3) regerr();  break;
	}
}

ATYPE	checkaddr( valu_t addr )
/* Called by functions emit_ad() and branch().	*/
{	ATYPE	addr_struct;

	addr_struct.seg = addr >> 16;
	addr_struct.off = addr & 0xFFFF;
	if ( addr_struct.seg < 0 )  addr_struct.seg = 0;
#ifdef ASLD
	else  fit(fit7(addr_struct.seg));
#endif
	return( addr_struct );
}

void emit_ad(expr_t ad_inf)
/* When the type of an operand is 'da' or 'x' this function
** emits the address.
*/
{	ATYPE	addr;

	addr = checkaddr( ad_inf.val );
		/* Always the long format is emitted, because the binary
		** will be downloaded into one z8000-segment with offset
		** 0x0000 upto 0xFFFF,so the chance we can use the short
		** format is very small.
		*/
	emit2( 1<<15 | addr.seg<<8 );
	emit2( addr.off );		/* ??? relocation information ??? */
}


void ldmcode(int wrd1,int wrd2, int num)
{	fit(fit4(num-1));
	emit2( mode | wrd1 );
	emit2( wrd2<<8 | num-1 );
	if ( mode>>12 == 4 ) emit_ad( addr_inf );
}


valu_t	adjust(valu_t absval)
{	valu_t	val = absval - DOTVAL - 2;

	if ( pass == PASS_2 && val > 0 ) val -= DOTGAIN;
	return( val );
}


void branch(int opc,expr_t exp)
/* This routine determines for the F3 format instructions whether the
** relative address is small enough to fit in normal code; If this is
** so normal code is emitted otherwise 'long' code is emitted contai-
** ning the direct address.
*/
{	int	longopc = 0, reladdr = 0, sm2, sm4;
	valu_t	val;
	ATYPE	addr;

	val = adjust(exp.val) >> 1;
	if ( (exp.typ & ~S_DOT) != DOTTYP )  sm2 = sm4 = 0;
	else
	{   switch ( opc & 0xF000 )
	    {   case DJNZ_: sm2 = fit7( -val );
			    reladdr = -val;
			    break;
		case JR_:   sm2 = fits8( val );
			    reladdr = low8( val );
			    longopc = 0x5E00 | (opc>>8 & 0xF);
			    break;
		case CALR_: sm2 = fits12( -val );
			    reladdr = low12( -val );
			    longopc = 0x5F00;
			    break;
	    }
	    sm4 = sm2 || fit8( (int)exp.val );
	}
	switch ( opc & 0xF000 )
	{   case DJNZ_: fit( sm2 );  /* djnz must be short */
			emit2( opc | reladdr );
			break;
	    case JR_: case CALR_:
			sm4 = small(sm4, 2);
			sm2 = small(sm2, 2);
			if ( sm2 )  emit2( opc | reladdr );
			else  /* replace by jp/call da */
			{   emit2( longopc );
			    addr = checkaddr( exp.val );
			    if ( sm4 )  /* short-offset */
				emit2( addr.seg<<8 | addr.off );
			    else
			    {   emit2( 1<<15 | addr.seg<<8 );
				emit2( addr.off );
			    }
			}
	}
}


void ldrel(int opc,expr_t exp)
/* This routine determines for the F4 format instructions whether the
** address is within the same segment  (meaning a relative address of
** less than 16 bits); If this is so normal code is emitted otherwise
** an error message is given.
*/
{	if ( pass >= PASS_2 && (exp.typ & ~S_DOT) != DOTTYP )
				serror( "relative too far" );
	emit2( opc );
	emit2( (int)adjust(exp.val) );
}


void shiftcode(int w1, int w2)
{   switch( w1 & 0x0F04 )
    {   /* Remember: w2 negative means right shift !   */
        case 0x200: /*byte*/  fit( w2>=-8 && w2<=8 );  break;
        case 0x300: /*word*/  fit( w2>=-16 && w2<=16 );  break;
        case 0x304: /*long*/  fit( w2>=-32 && w2<=32 );  break;
    }
    emit2( w1 );
    emit2( w2 );
}


void argerr(void)
{	serror( "illegal operand" );	}
