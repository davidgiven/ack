/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Header$"

/*
 * VAX-11 Machine dependent C declarations
 */

static
oprnd(p)
	register struct operand *p;
{
	/*	Process one operand */

	int	sm;

	if (p->index_reg >= 0 && p->mode != DISPL) {
		/* indexed mode; emit */
		emit1((INDEX_MODE << 4) | p->index_reg);
	}

	switch(p->mode) {
	case REG_MODE:
		emit1((REG_MODE << 4) | p->reg);
		break;
	case REGDEF_MODE:
		emit1((REGDEF_MODE << 4) | p->reg);
		break;
	case AI_MODE:
		emit1((AI_MODE << 4) | p->reg);
		break;
	case AI_DEF_MODE:
		emit1((AI_DEF_MODE << 4) | p->reg);
		break;
	case AD_MODE:
		emit1((AD_MODE << 4) | p->reg);
		break;
	case DISPLL_MODE:
	case DISPLL_DEF_MODE:
		if (small(p->exp.typ == S_ABS && fitw(p->exp.val), 2)) {
			if (small(fitb(p->exp.val), 1)) {
				/* DISPLB_MODE or DISPLB_DEF_MODE */
				emit1(((p->mode-4)<<4) | p->reg);
				emit1((int)(p->exp.val));
			}
			else {
				/* DISPLW_MODE or DISPLW_DEF_MODE */
				emit1(((p->mode-2)<<4) | p->reg);
				emit2((int)(p->exp.val));
			}
		}
		else {
			small(0, 1);	/* dummy call */
			emit1((p->mode<<4) | p->reg);
#ifdef RELOCATION
			RELOMOVE(relonami, p->relo);
			newrelo(p->exp.typ, RELO4);
#endif
			emit4((long) p->exp.val);
		}
		break;
	case DISPL:
		/* a displacement */
		p->exp.val -= (DOTVAL + p->size);
		if ((pass == PASS_2) &&
		    (p->exp.val > 0) &&
		    ((p->exp.typ & S_DOT) == 0)
		   ) {
			p->exp.val -= DOTGAIN;
		}
		if (p->size == 1) sm = fitb(p->exp.val);
		else if (p->size == 2) sm = fitw(p->exp.val);
		else sm = 1;
		if (pass >= PASS_2 &&
		    ((p->exp.typ & ~S_DOT) != DOTTYP || !sm)) {
			serror("label too far");
		}
		if (p->size == 1) emit1((int)(p->exp.val));
		else if (p->size == 2) emit2((int)(p->exp.val));
		else emit4(p->exp.val);
		break;
	case IMM:
		if (p->size < 0) {
			serror("immediate mode not allowed here");
			p->size = 4;
		}
		else if (p->size == 0) {
			serror("this immediate mode is not implemented");
			p->size = 4;
		}
		if (small(p->exp.typ == S_ABS && literal(p->exp.val), p->size)){
			emit1((int)(p->exp.val));
		}
		else {
			emit1((AI_MODE << 4) | PC);
			RELOMOVE(relonami, p->relo);
			switch(p->size) {
			case 1:
#ifdef RELOCATION
				newrelo(p->exp.typ, RELO1);
#endif
				emit1((int)(p->exp.val));
				break;
			case 2:
#ifdef RELOCATION
				newrelo(p->exp.typ, RELO2);
#endif
				emit2((int)(p->exp.val));
				break;
			case 4:
#ifdef RELOCATION
				newrelo(p->exp.typ, RELO4);
#endif
				emit4(p->exp.val);
				break;
			default:
				assert(0);
			}
		}
		break;
	case ABS:
	case ABS_DEF:
		if (p->mode == ABS) p->mode = DISPLL_MODE;
		else p->mode = DISPLL_DEF_MODE;
		p->reg = PC;
		p->exp.val -= (DOTVAL + 2);
		if ((pass == PASS_2)
		    &&
		    (p->exp.val > 0)
		    &&
		    ((p->exp.typ & S_DOT) == 0)
		   ) {
			p->exp.val -= DOTGAIN;
		}
		sm = fitw(p->exp.val - 1);
		if ((p->exp.typ & ~S_DOT) != DOTTYP) sm = 0;
		if (small(sm, 2)) {
			if (small(fitb(p->exp.val), 1)) {
				/* DISPLB_MODE or DISPLB_DEF_MODE */
				emit1(((p->mode-4)<<4) | p->reg);
				emit1((int)(p->exp.val));
			}
			else {
				/* DISPLW_MODE or DISPLW_DEF_MODE */
				emit1(((p->mode-2)<<4) | p->reg);
				emit2((int)(p->exp.val - 1));
			}
		}
		else {
			small(0, 1);	/* dummy call */
			emit1((p->mode<<4) | p->reg);
#ifdef RELOCATION
			RELOMOVE(relonami, p->relo);
			newrelo(p->exp.typ, RELO4|RELPC);
#endif
			emit4((long) p->exp.val - 3);
		}
		break;
	default:
		assert(0);
	}
}

static int
size_ops()
{
	/*	Give an upper bound on the size of the operands
	*/
	register struct operand *p = &opnd[0];
	register int i;
	register int sz = 0;

	for (i = 0; i < op_ind; i++) {
		if (p->index_reg >= 0 && p->mode != DISPL) {
			sz++;
		}
		switch(p->mode) {
		case REG_MODE:
		case REGDEF_MODE:
		case AI_MODE:
		case AI_DEF_MODE:
		case AD_MODE:
			sz++;
			break;
		case DISPLL_MODE:
		case DISPLL_DEF_MODE:
		case ABS:
		case ABS_DEF:
		case IMM:
			sz += 5;
			break;
		case DISPL:
			sz += p->size;
			break;
		default:
			assert(0);
		}
		p++;
	}
	return sz;
}

branch(opc, exp)
	expr_t exp;
{
	exp.val -= (DOTVAL + 2);
	if ((pass == PASS_2) &&
	    (exp.val > 0) &&
	    ((exp.typ & S_DOT) == 0)
	   ) {
		exp.val -= DOTGAIN;
	}
	if (pass >= PASS_2 &&
	    ((exp.typ & ~S_DOT) != DOTTYP || ! fitw(exp.val))) {
		serror("label too far");
	}
	if (small(fitb(exp.val) && ((exp.typ & ~S_DOT) == DOTTYP), 1)) {
		emit1(opc);
		emit1((int) exp.val);
	}
	else {
		emit1(opc|0x20);
		emit2((int) exp.val);
	}
}

ext_branch(opc, exp)
	expr_t exp;
{
	int sm;
	int gain = opc == 0x11 ? 1 : 3;
	valu_t	val, d2 = DOTVAL + 2;

	exp.val -= d2;
	if ((pass == PASS_2) &&
	    (exp.val > 0) &&
	    ((exp.typ & S_DOT) == 0)
	   ) {
		exp.val -= DOTGAIN;
	}
	if (exp.val < 0) val = exp.val - size_ops();
	else val = exp.val;
	sm = fitw(val);
	if ((exp.typ & ~S_DOT) != DOTTYP) sm = 0;
	if (small(sm, 3)) {
		if (small(fitb(val), gain)) {
			emit1(opc);
			operands(op_ind);
			emit1((int) (exp.val - (DOTVAL + 1 - d2)));
		}
		else {
			if (opc != 0x11) {
				emit1(opc^1);
				operands(op_ind);
				emit1(3);
			}
			emit1(0x31);
			emit2((int) (exp.val - (DOTVAL + 2 - d2)));
		}
	}
	else {
		small(0, gain);	/* dummy call */
		if (opc != 0x11) {
			emit1(opc ^ 1);
			operands(op_ind);
			emit1(6);
		}
		emit1(0x17);	/* jmp */
		emit1((DISPLL_MODE << 4) | PC);
#ifdef RELOCATION
		newrelo(exp.typ, RELO4|RELPC);
#endif
		emit4(exp.val - (DOTVAL + 4 - d2));
	}
}

operands(cnt)
{
	register int i;

	for (i = 0; i < cnt; i++) {
		oprnd(&opnd[i]);
	}
}
