/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Id$"

/*
 * VAX-11 Machine dependent C declarations
 */

/* Opcode of branch on reversed condition. */
#define rev_cond_branch(opc)	((opc) ^ 1)

/* Process one operand. */
static
oprnd(p)
	register struct operand *p;
{
	int	sm;

	if (p->index_reg >= 0 && p->mode != DISPL) {
		/* Indexed mode; emit */
		emit1((INDEX_MODE << 4) | p->index_reg);
	}

	switch(p->mode) {
	case REG_MODE:
		if (p->size == -2 && p->index_reg < 0) {
			serror("register mode not allowed here");
		}
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
		/* Three possible sizes: 1, 2, and 4 (and 0, but this is
		   not implemented). Therefore, we need two bits in the
		   optimize table.
		*/
		if (small(p->exp.typ == S_ABS && fitw(p->exp.val), 2)) {
			/* We gained two bytes; see if we can gain another. */
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
		else {	/* We need 4 bytes here. */
			small(0, 1);	/* dummy call too keep bits in sync */
			emit1((p->mode<<4) | p->reg);
#ifdef RELOCATION
			RELOMOVE(relonami, p->relo);
			newrelo(p->exp.typ, RELO4);
#endif
			emit4((long) p->exp.val);
		}
		break;
	case DISPL:
		/* A displacement. The p->size field contains the size. */
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
		else {
#ifdef RELOCATION
			RELOMOVE(relonami, p->relo);
			newrelo(p->exp.typ, RELO4|RELPC);
#endif
			emit4(p->exp.val);
		}
		break;
	case IMM:
		/* Immediate mode; either literal mode or auto-increment
		   of program counter.
		*/
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
		/* Absolute mode (is auto-increment deferred with respect
		   to the program counter).
		*/
		emit1((AI_DEF_MODE << 4) | PC);
#ifdef RELOCATION
		RELOMOVE(relonami, p->relo);
		newrelo(p->exp.typ, RELO4);
#endif
		emit4(p->exp.val);
		break;
	case REL:
	case REL_DEF:
		/* Relative or relative deferred is actually displacement
		   or displacement deferred, but relative to program counter.
		*/
		if (p->mode == REL) p->mode = DISPLL_MODE;
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
		/* Why test for exp.val - 1? Well, if we need a word for
		   the offset, we actually generate one byte more, and this
		   is reflected in the value of the program counter.
		*/
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
				/* exp.val - 1: see comment above */
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
			/* exp.val - 3: see comment above */
			emit4((long) p->exp.val - 3);
		}
		break;
	default:
		assert(0);
	}
}

/* Give an upper bound on the size of the operands */
static int
size_ops()
{
	register struct operand *p = &opnd[0];
	register int i;
	register int sz = 0;

	for (i = op_ind; i > 0; i--) {
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
		case REL:
		case REL_DEF:
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

/* Branch with byte or word offset  */
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
	/* For the reason of exp.val-1, see the comment at the generation
	   of the RELative addressing mode.
	*/
	if (pass >= PASS_2 &&
	    ((exp.typ & ~S_DOT) != DOTTYP || ! fitw(exp.val - 1))) {
		serror("label too far");
	}
	if (small(fitb(exp.val) && ((exp.typ & ~S_DOT) == DOTTYP), 1)) {
		emit1(opc);
		emit1((int) exp.val);
	}
	else {
		emit1(opc|0x20);
		emit2((int) (exp.val - 1));
	}
}

/* Extended conditional branch instructions: if offset is too far,
   they are replaced by a reversed conditional branch over a word-branch or
   jump.
*/
ext_branch(opc, exp)
	expr_t exp;
{
	int sm;
	int gain = opc == BRB ? 1 : 3;
	valu_t	val, d2 = DOTVAL + 2;

	exp.val -= d2;
	if ((pass == PASS_2) &&
	    (exp.val > 0) &&
	    ((exp.typ & S_DOT) == 0)
	   ) {
		exp.val -= DOTGAIN;
	}
	/* We have not generated the operands yet and cannot do so
	   because we don't know the opcode yet and have to generate that
	   first. Therefore, we make a conservative guess of the size
	   of the operands in case the branch is backwards. If it is
	   forwards, the (sizes of the) operands do not matter.
	*/
	if (exp.val < 0) val = exp.val - size_ops();
	else val = exp.val;
	sm = fitw(val);
	if ((exp.typ & ~S_DOT) != DOTTYP) sm = 0;
	/* We gain three bytes if the offset fits in a word; for a
	   jump we also need an addressing mode byte.
	*/
	if (small(sm, 3)) {
		/* Here we can gain 3 bytes if the extended branch is
		   conditional and the offset fits in a byte. Otherwise,
		   if the offset fits in a byte we gain 1 byte.
		*/
		if (small(fitb(val), gain)) {
			emit1(opc);
			operands();
			/* Adjust exp.val for operand sizes. Keep into account
			   that we already generated the opcode(!). This
			   accounts for the "+ 1" instead of "+ 2".
			*/
			emit1((int) (exp.val - (DOTVAL + 1 - d2)));
		}
		else {
			if (opc != BRB) {
				emit1(rev_cond_branch(opc));
				operands();
				emit1(3);
			}
			emit1(BRW);
			emit2((int) (exp.val - (DOTVAL + 2 - d2)));
		}
	}
	else {
		small(0, gain);	/* dummy call to keep bittab in sync */
		if (opc != BRB) {
			emit1(rev_cond_branch(opc));
			operands();
			emit1(6);
		}
		emit1(JMP);
		emit1((DISPLL_MODE << 4) | PC);
#ifdef RELOCATION
		newrelo(exp.typ, RELO4|RELPC);
#endif
		emit4(exp.val - (DOTVAL + 4 - d2));
	}
}

/* Generate code for the operands */
operands()
{
	register int i;

	for (i = 0; i < op_ind; i++) {
		oprnd(&opnd[i]);
	}
}
