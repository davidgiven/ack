/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#define maskx(v, x) (v & ((1<<(x))-1))

/* Assemble an ALU instruction where rb is a register. */

void alu_instr_reg(quad op, quad cc, quad rd, quad ra, quad rb)
{
	/* Can we use short form? */

	if ((cc == ALWAYS) && (ra == rd))
	{
		emit2(B16(01000000,00000000) | (op<<8) | (rb<<4) | (rd<<0));
		return;
	}

    /* Long form, then. */

    emit2(B16(11000000,00000000) | (op<<5) | (rd<<0));
    emit2(B16(00000000,00000000) | (ra<<11) | (cc<<7) | (rb<<0));
}

/* Assemble an ALU instruction where rb is a literal. */

void alu_instr_lit(quad op, quad cc, quad rd, quad ra, quad value)
{
	/* 16 bit short form? */

	if ((cc == ALWAYS) && !(op & 1) && (value <= 0x1f) && (ra == rd) &&
		!(ra & 0x10))
	{
		emit2(B16(01100000,00000000) | (op<<8) | (value<<4) | (rd<<0));
		return;
	}

	/* 32 bit medium form? */

    if (value >= 0x1f)
    {
        emit2(B16(11000000,00000000) | (op<<5) | (rd<<0));
        emit2(B16(00000000,01000000) | (ra<<11) | (cc<<7) | (value<<0));
        return;
    }

	/* Long form, then. */

	if (cc != ALWAYS)
		serror("cannot use condition codes with ALU literals this big");

	/* add is special. */

	if (op == B8(00000010))
		emit2(B16(11101100,00000000) | (ra<<5) | (rd<<0));
	else
	{
		if (ra != rd)
			serror("can only use 2op form of ALU instructions with literals this big");
		emit2(B16(11101000,00000000) | (op<<5) | (rd<<0));
	}

	emit4(value);
}

/* Miscellaneous instructions with three registers and a cc. */

void misc_instr_reg(quad op, quad cc, quad rd, quad ra, quad rb)
{
	emit2(op | (rd<<0));
	emit2(B16(00000000,00000000) | (ra<<11) | (cc<<7) | (rb<<0));
}

/* Miscellaneous instructions with two registers, a literal, and a cc. */

void misc_instr_lit(quad op, quad cc, quad rd, quad ra, quad value)
{
    if (value < 0x1f)
        serror("only constants from 0..31 can be used here");

	emit2(op | (rd<<0));
	emit2(B16(00000000,01000000) | (ra<<11) | (cc<<7) | (value<<0));
}

/* Assemble a branch instruction. This may be a near branch into this
 * object file, or a far branch which requires a fixup. */

void branch_instr(quad bl, quad cc, struct expr_t* expr)
{
	quad type = expr->typ & S_TYP;

	/* Sanity checking. */

	if (bl && (cc != ALWAYS))
		serror("can't use condition codes with bl");
	if (type == S_ABS)
		serror("can't use absolute addresses here");

	switch (pass)
	{
		case 0:
			/* Calculate size of instructions only. For now we just assume
			 * that they're going to be the maximum size, 32 bits. */

			emit4(0);
			break;

		case 1:
		case 2:
		{
			/* The VC4 branch instructions express distance in 2-byte
			 * words. */

			int d = (expr->val - DOTVAL) / 2;

        	/* We now know the worst case for the instruction layout. At
        	 * this point we can emit the instructions, which may shrink
        	 * the code. */

			if (!bl && (type == DOTTYP))
			{
        	    /* This is a reference to code within this section. If it's
        	     * close enough to the program counter, we can use a short-
        	     * form instruction. */

        	    if ((d >= -128) && (d < 127))
        	    {
					emit2(B16(00011000,00000000) | (cc<<7) | (d&0x7f));
					break;
				}
			}

			/* Absolute addresses and references to other sections
			 * need the full 32 bits. */

			newrelo(expr->typ, RELOVC4 | RELPC);

			if (bl)
			{
				quad v = d & 0x07ffffff;
				quad hiv = v >> 23;
				quad lov = v & 0x007fffff;
				emit2(B16(10010000,10000000) | (lov>>16) | (hiv<<8));
				emit2(B16(00000000,00000000) | (lov&0xffff));
			}
			else
			{
				quad v = d & 0x007fffff;
				emit2(B16(10010000,00000000) | (cc<<8) | (v>>16));
				emit2(B16(00000000,00000000) | (v&0xffff));
			}
			break;
        }
	}
}

/* Push/pop. */

void stack_instr(quad opcode, quad loreg, quad hireg, quad extrareg)
{
    quad b;

    switch (loreg)
    {
        case 0: b = 0; break;
        case 6: b = 1; break;
        case 16: b = 2; break;
        case 24: b = 3; break;

        case 26: /* lr */
            extrareg = 26;
            hireg = 31;
            loreg = 0;
            b = 0;
            break;

		case 31: /* pc */
			extrareg = 31;
			hireg = 31;
			loreg = 0;
			b = 0;
			break;

		default:
			serror("base register for push or pop may be only r0, r6, r16, r24, lr or pc");
	}

	if (opcode & 0x0080)
	{
		/* Pop */
		if (extrareg == 26)
			serror("cannot pop lr");
	}
	else
	{
		/* Push */
		if (extrareg == 31)
			serror("cannot push pc");
	}

	if (hireg < loreg)
		serror("invalid register range");

	emit2(opcode | (b<<5) | (hireg<<0) |
		((extrareg != -1) ? 0x0100 : 0));
}

/* Memory operations where the offset is a fixed value (including zero). */

void mem_instr(quad opcode, quad cc, quad rd, long offset, quad rs)
{
	quad uoffset = (quad) offset;
	int multiple4 = !(offset & 3);
	int intonly = ((opcode & B8(00000110)) == 0);

	/* If no CC, there are some special forms we can use. */

	if (cc == ALWAYS)
	{
		/* Very short form, special for stack offsets. */

		if (intonly && (rs == 25) && multiple4 && fitx(offset, 7) && (rd < 0x10))
		{
			quad o = maskx(offset, 7) / 4;
			emit2(B16(00000100,00000000) | (opcode<<9) | (o<<4) | (rd<<0));
			return;
		}

		/* Slightly longer form for directly dereferencing via a register. */

		if ((rs < 0x10) && (rd < 0x10) && (offset == 0))
		{
			emit2(B16(00001000,00000000) | (opcode<<8) | (rs<<4) | (rd<<4));
			return;
		}

	    /* Integer only, but a limited offset. */

	    if (intonly && (uoffset <= 0x3f) && (rs < 0x10) && (rd < 0x10))
		{
			quad o = uoffset / 4;
			emit2(B16(00100000,00000000) | (opcode<<12) | (o<<8) |
				(rs<<4) | (rd<<0));
			return;
		}

		/* Certain registers support 16-bit offsets. */

		if (fitx(offset, 16))
		{
			switch (rs)
			{
                case 0: opcode = B16(10101011,00000000) | (opcode<<5); goto specialreg;
                case 24: opcode = B16(10101000,00000000) | (opcode<<5); goto specialreg;
                case 25: opcode = B16(10101001,00000000) | (opcode<<5); goto specialreg;
                case 31: opcode = B16(10101010,00000000) | (opcode<<5); goto specialreg;
                default: break;

                specialreg:
                {
                    quad o = maskx(offset, 16);
                    emit2(opcode | (rd<<0));
                    emit2(o);
                    return;
                }
			}
		}

        /* 12-bit displacements. */

        if (fitx(offset, 12))
        {
        	quad looffset = maskx(offset, 11);
        	quad hioffset = (offset >> 11) & 1;

        	emit2(B16(10100010,00000000) | (opcode<<5) | (rd<<0) | (hioffset<<8));
        	emit2(B16(00000000,00000000) | (rs<<11) | (looffset<<0));
        	return;
        }

        /* Everything else uses Very Long Form. */

		if (!fitx(offset, 27))
			serror("offset will not fit into load/store instruction");

		if (rs == 31)
			opcode = B16(11100111,00000000) | (opcode<<5);
		else
			opcode = B16(11100110,00000000) | (opcode<<5);

		emit2(opcode | (rd<<0));
		emit4((rs<<27) | maskx(offset, 27));
		return;
	}

	/* Now we're on to load/store instructions with ccs. */

	if (uoffset <= 0x1f)
	{
		emit2(B16(10100000,00000000) | (opcode<<5) | (rd<<0));
		emit2(B16(00000000,01000000) | (rs<<11) | (cc<<7) | (uoffset<<0));
		return;
	}

	/* No encoding for this instruction. */

	serror("invalid load/store instruction");
}

