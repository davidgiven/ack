/* $Header: mach5.c, v1.6 3-Nov-88 AJM */

branch(brtyp, link, val)
word_t brtyp;
word_t link;
valu_t val;
{
	valu_t offset;

	offset = val - DOTVAL - 8;
	if ((offset & 0xFC000000) != 0 && (offset & 0xFC000000) != 0xFC000000){
		serror("offset out of range");
	}
	offset = offset>>2 & 0xFFFFFF; 
	emit4(brtyp|link|offset);
	return;
}

data(opc, ins, val, typ)
long opc, ins;
valu_t val;
short typ;
{
	valu_t tmpval;

	if (typ == S_REG){
		emit4(opc|ins|val);
		return;
	}

	ins |= 0x02000000;

	tmpval = val;
	if (typ == S_ABS){
		if (calcimm(&opc, &tmpval, typ)){
			emit4(opc|ins|tmpval);
			return;
		}
	}

	tmpval = val;
	if (small(calcimm(&opc, &tmpval, typ),12)){
		emit4(opc|ins|tmpval);
		return;
	}	

	if (opc == MOV && typ != S_ABS)
	{
		if (small((val & 0xF0000000) == 0xF0000000, 8)){
			emit4(0xE51F0004 | (ins & 0xF000));
			emit4(val);
			return;
		}
		if (small(1,4)){
			emit4(0xE51F0000 | (ins & 0xF000));
			emit4(0xEA000000);
			emit4(val);
			return;
		}
		DOTVAL += 16;
		return;
	}
	if (opc == ADD && typ != S_ABS)
	{
		if (small((val & 0xF0000000) == 0xF0000000, 4)){
			emit4(0xE51F0004 | (ins & 0xF000));
			emit4(val);
			emit4(0xE2800000 | (ins&0xFF000) | (ins&0xF000)>>12);
			return;
		}
		emit4(0xE51F0000 | (ins & 0xF000));
		emit4(0xEA000000);
		emit4(val);
		emit4(0xE2800000 | (ins&0xFF000) | (ins&0xF000)>>12);
		return;
	}
	/* default: */
		if (pass == PASS_1)
			DOTVAL += 16;
		else
			serror("immediate value out of range");
		return;

}

calcimm(opc,val,typ)
word_t *opc;
valu_t *val;
short typ;
{
	int i = 0;

	if (typ == S_UND) return 0;

	if ((*val & 0xFFFFFF00) == 0) return 1;

	if ((~*val & 0xFFFFFF00) == 0){
		if (*opc == AND)
			{
			*val = ~*val;
			*opc = BIC;
			return 1;
			}
		if (*opc == MOV)
			{
			*val = ~*val;
			*opc = MVN;
			return 1;
			}
		if (*opc == ADC)
			{
			*val = ~*val;
			*opc = SBC;
			return 1;
			}

	}	
	if ((-1**val & 0xFFFFFF00) == 0 ){
		if (*opc == ADD)
			{
			*val *= -1;
			*opc = SUB;
			return 1;
			}
		if (*opc == CMP)
			{
			*val *= -1;
			*opc = CMN;
			return 1;
			}
	}

	do{
		rotateleft2(&*val);
		i++;
		if((*val & 0xFFFFFF00) == 0){
			*val = *val|i<<8;
			return 1;
		}
		if ((~*val & 0xFFFFFF00) == 0){
			if (*opc == AND)
				{
				*val = ~*val|i<<8;
				*opc = BIC;
				return 1;
				}
			if (*opc == MOV)
				{
				*val = ~*val|i<<8;
				*opc = MVN;
				return 1;
				}
			if (*opc == ADC)
				{
				*val = ~*val|i<<8;
				*opc = SBC;
				return 1;
				}
		}	
	}while(i<15);

	return 0;
}

word_t
calcoffset(val)
valu_t val;
{
	if((val & 0xFFFFF000) == 0)
		return(val|0x00800000);
	val *= -1;
	if((val & 0xFFFFF000) == 0)
		return(val);
	serror("offset out of range");
	return(0);
}

word_t
calcaddress (val,typ,reg)
valu_t val;
short typ;
word_t reg;
{
	int tmpval;

	if (typ == S_UND){
		DOTVAL += 8;
		return 0;
	}
	tmpval = val - DOTVAL - 8;
	if(small((tmpval & 0xFFFFF000) == 0, 8))
		return(val|0x008F0000);
	tmpval *= -1;
	if(small((tmpval & 0xFFFFF000) == 0, 8))
		return(val|0x000F0000);
	emit4(0xE51F0004 | reg << 12);
	emit4(val | 0xF0000000);
	return(reg << 16);
}

word_t
calcadr(ins, reg, val, typ)
word_t ins, reg;
valu_t val;
short typ;
{
	valu_t tmpval = val;
	word_t opc;
	int i = 0;

	if (typ != S_ABS){
		tmpval = val-DOTVAL-8;
		if (tmpval > 0) {
			if (small((tmpval & 0xFFFFFF00) == 0),12){
				emit4(ins|ADD|0x020F0000|reg<<12|tmpval);
				return 0;
			}
		}
	
		tmpval *= -1;
		if (small((tmpval & 0xFFFFFF00) == 0), 12){
			emit4(ins|SUB|0x020F0000|reg<<12|tmpval);
			return 0;
		}
	}

	tmpval = val;
	opc = MOV;
	if (calcimm(&opc, &tmpval, typ)){
		emit4(ins|opc|0x020F0000|reg<<12|tmpval);
		return 0;
	}

/* Failed */
	if (pass == PASS_1)
		DOTVAL += 16;
	else
		serror("illegal ADR argument");
	return ;
}


word_t
calcshft(val, typ, styp)
valu_t val;
short typ;
word_t styp;
{
	if (typ=S_UND) return 0;
	if (val & 0xFFFFFFE0) serror("shiftcount out of range");
	if (styp && !val) warning("shiftcount 0");
	return((val & 0x1F)<<7);
}

rotateleft2(x)
long *x;
{
	unsigned long bits;

	bits = *x & 0xC0000000;
	*x <<= 2 ;
	if (bits){
		bits >>= 30;
		*x |= bits;
	}
	return;
}
