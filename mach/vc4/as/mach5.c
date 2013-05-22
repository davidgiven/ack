/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include <stdint.h>

#define maskx(v, x) (v & ((1<<(x))-1))

static void toobig(void)
{
	serror("offset too big to encode into instruction");
}

/* Assemble an ALU instruction where rb is a register. */

void alu_instr_reg(quad op, int cc, int rd, int ra, int rb)
{
	/* Can we use short form? */

	if ((cc == ALWAYS) && (ra == rd) && (ra < 0x10) && (rb < 0x10))
	{
		emit2(B16(01000000,00000000) | (op<<8) | (rb<<4) | (rd<<0));
		return;
	}

    /* Long form, then. */

    emit2(B16(11000000,00000000) | (op<<5) | (rd<<0));
    emit2(B16(00000000,00000000) | (ra<<11) | (cc<<7) | (rb<<0));
}

/* Assemble an ALU instruction where rb is a literal. */

void alu_instr_lit(quad op, int cc, int rd, int ra, quad value)
{
	/* 16 bit short form? */

	if ((cc == ALWAYS) && !(op & 1) && (value <= 0x1f) && (ra == rd) &&
		(ra < 0x10))
	{
		emit2(B16(01100000,00000000) | (op<<8) | (value<<4) | (rd<<0));
		return;
	}

	/* 32 bit medium form? */

    if (value <= 0x1f)
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

void misc_instr_reg(quad op, int cc, int rd, int ra, int rb)
{
	emit2(op | (rd<<0));
	emit2(B16(00000000,00000000) | (ra<<11) | (cc<<7) | (rb<<0));
}

/* Miscellaneous instructions with two registers, a literal, and a cc. */

void misc_instr_lit(quad op, int cc, int rd, int ra, quad value)
{
    if (value < 0x1f)
        serror("only constants from 0..31 can be used here");

	emit2(op | (rd<<0));
	emit2(B16(00000000,01000000) | (ra<<11) | (cc<<7) | (value<<0));
}

/* Assemble a branch instruction. This may be a near branch into this
 * object file, or a far branch which requires a fixup. */

void branch_instr(int bl, int cc, struct expr_t* expr)
{
	quad pc = DOTVAL;
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

			int d = ((int32_t)expr->val - (int32_t)pc) / 2;

        	/* We now know the worst case for the instruction layout. At
        	 * this point we can emit the instructions, which may shrink
        	 * the code. */

			if (!bl && (type == DOTTYP))
			{
        	    /* This is a reference to code within this section. If it's
        	     * close enough to the program counter, we can use a short-
        	     * form instruction. */

        	    if (fitx(d, 7))
        	    {
					emit2(B16(00011000,00000000) | (cc<<7) | (d&0x7f));
					break;
				}
			}

			/* Absolute addresses and references to other sections
			 * need the full 32 bits. */

			newrelo(expr->typ, RELOVC4|RELPC);

			if (bl)
			{
				quad v, hiv, lov;

				if (!fitx(d, 27))
					toobig();

				v = maskx(d, 27);
				hiv = v >> 23;
				lov = v & 0x007fffff;
				emit2(B16(10010000,10000000) | (lov>>16) | (hiv<<8));
				emit2(B16(00000000,00000000) | (lov&0xffff));
			}
			else
			{
				quad v;

				if (!fitx(d, 23))
					toobig();

				v = maskx(d, 23);
				emit2(B16(10010000,00000000) | (cc<<8) | (v>>16));
				emit2(B16(00000000,00000000) | (v&0xffff));
			}
			break;
        }
	}
}

/* Push/pop. */

void stack_instr(quad opcode, int loreg, int hireg, int extrareg)
{
    int b;
    int m;

    switch (loreg)
    {
        case 0: b = 0; break;
        case 6: b = 1; break;
        case 16: b = 2; break;
        case 24: b = 3; break;

        case 26: /* lr */
            extrareg = 26;
            hireg = loreg = -1;
            b = 0;
            break;

		case 31: /* pc */
			extrareg = 31;
			hireg = loreg = -1;
			b = 0;
			break;

		default:
			serror("base register for push or pop may be only r0, r6, r16, r24, lr or pc");
	}

	if (opcode & 0x0080)
	{
		/* Push */
		if (extrareg == 31)
			serror("cannot push pc");
	}
	else
	{
		/* Pop */
		if (extrareg == 26)
			serror("cannot pop lr");
	}

	if (hireg < loreg)
		serror("invalid register range");

	if (hireg == -1)
		m = 31;
	else
		m = hireg - loreg;

	emit2(opcode | (b<<5) | (m<<0) | ((extrareg != -1) ? 0x0100 : 0));
}

/* Memory operations where the offset is a fixed value (including zero). */

void mem_instr(quad opcode, int cc, int rd, long offset, int rs)
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
			emit2(B16(00001000,00000000) | (opcode<<8) | (rs<<4) | (rd<<0));
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

/* Memory operations where the destination address is a sum of two
 * registers. */

void mem_offset_instr(quad opcode, int cc, int rd, int ra, int rb)
{
    emit2(B16(10100000,00000000) | (opcode<<5) | (rd<<0));
    emit2(B16(00000000,00000000) | (ra<<11) | (cc<<7) | (rb<<0));
}

/* Memory operations with postincrement. */

void mem_postincr_instr(quad opcode, int cc, int rd, int rs)
{
    emit2(B16(10100101,00000000) | (opcode<<5) | (rd<<0));
    emit2(B16(00000000,00000000) | (rs<<11) | (cc<<7));
}

/* Memory operations where the destination is an address literal. */

void mem_address_instr(quad opcode, int rd, struct expr_t* expr)
{
	static const char sizes[] = {4, 2, 1, 2};
	int size = sizes[opcode];
	quad type = expr->typ & S_TYP;

	/* Sanity checking. */

	if (type == S_ABS)
		serror("can't use absolute addresses here");

	switch (pass)
	{
		case 0:
			/* Calculate size of instructions only. For now we just assume
			 * that they're going to be the maximum size, 48 bits. */

			emit2(0);
			emit4(0);
			break;

		case 1:
		case 2:
		{
			int d = expr->val - DOTVAL;

        	/* We now know the worst case for the instruction layout. At
        	 * this point we can emit the instructions, which may shrink
        	 * the code. */

			if (type == DOTTYP)
			{
				int scaledd = d/size;

        	    /* This is a reference to an address within this section. If
        	     * it's close enough to the program counter, we can use a
        	     * shorter instruction. */

				if (fitx(scaledd, 16))
				{
                    emit2(B16(10101010,00000000) | (opcode<<5) | (rd<<0));
                    emit2(scaledd);
                    return;
                }
			}

			/* Otherwise we need the full 48 bits. */

			newrelo(expr->typ, RELOVC4|RELPC);

			/* VC4 relocations store the PC-relative delta into the
			 * destination section in the instruction data. The linker will
			 * massage this, and scale it appropriately. */

            if (!fitx(d, 27))
				toobig();

            emit2(B16(11100111,00000000) | (opcode<<5) | (rd<<0));
            emit4((31<<27) | maskx(d, 27));
			break;
        }
	}
}

/* Common code for handling addcmp: merge in as much of expr as will fit to
 * the second pair of the addcmp opcode. */

static void branch_addcmp_common(quad opcode, int bits, struct expr_t* expr)
{
	quad type = expr->typ & S_TYP;

	switch (pass)
	{
		case 0:
			/* Calculate size of instructions only. */

			emit2(0);
			break;

		case 1:
		case 2:
		{
			if (type != DOTTYP)
				serror("can't use this type of branch to jump outside the section");

			/* The VC4 branch instructions express distance in 2-byte
			 * words. */

			int d = (expr->val - DOTVAL-2 + 4) / 2;

			if (!fitx(d, bits))
				serror("target of branch is too far away");

			emit2(opcode | maskx(d, bits));
			break;
        }
	}
}

void branch_addcmp_reg_reg_instr(int cc, int rd, int ra, int rs, struct expr_t* expr)
{
    if ((rd >= 0x10) || (ra >= 0x10) || (rs >= 0x10))
        serror("can only use r0-r15 in this instruction");

	emit2(B16(10000000,00000000) | (cc<<8) | (ra<<4) | (rd<<0));
	branch_addcmp_common(B16(00000000,00000000) | (rs<<10), 10, expr);
}

void branch_addcmp_lit_reg_instr(int cc, int rd, long va, int rs, struct expr_t* expr)
{
    if ((rd >= 0x10) || (rs >= 0x10))
        serror("can only use r0-r15 in this instruction");

	if (!fitx(va, 4))
		serror("value too big to encode into instruction");
    va = maskx(va, 4);

    emit2(B16(10000000,00000000) | (cc<<8) | (va<<4) | (rd<<0));
    branch_addcmp_common(B16(01000000,00000000) | (rs<<10), 10, expr);
}

void branch_addcmp_reg_lit_instr(int cc, int rd, int ra, long vs, struct expr_t* expr)
{
    if ((rd >= 0x10) || (ra >= 0x10))
        serror("can only use r0-r15 in this instruction");

	if (!fitx(vs, 6))
		serror("value too big to encode into instruction");
	vs = maskx(vs, 6);

	emit2(B16(10000000,00000000) | (cc<<8) | (ra<<4) | (rd<<0));
	branch_addcmp_common(B16(10000000,00000000) | (vs<<8), 8, expr);
}

void branch_addcmp_lit_lit_instr(int cc, int rd, long va, long vs, struct expr_t* expr)
{
    if (rd >= 0x10)
        serror("can only use r0-r15 in this instruction");

	if (!fitx(va, 4) || !fitx(vs, 6))
		serror("value too big to encode into instruction");
	va = maskx(va, 4);
	vs = maskx(vs, 6);

	emit2(B16(10000000,00000000) | (cc<<8) | (va<<4) | (rd<<0));
	branch_addcmp_common(B16(11000000,00000000) | (vs<<8), 8, expr);
}

/* lea, where the source is relative to the stack. */

void lea_stack_instr(int rd, long va, int rs)
{
    if (rs != 25)
        serror("source register must be sp");

	if (!fitx(va, 6))
		serror("offset too big to encode in instruction");
	va = maskx(va, 6);

	emit2(B16(00010000,00000000) | (rd<<0) | (va<<5));
}

/* lea, where the source is an address. */

void lea_address_instr(int rd, struct expr_t* expr)
{
	quad pc = DOTVAL;
	quad type = expr->typ & S_TYP;

	if (type == S_ABS)
		serror("can't use absolute addresses here");

	newrelo(expr->typ, RELOVC4|RELPC);

	/* VC4 relocations store the PC-relative delta into the
	 * destination section in the instruction data. The linker will
	 * massage this, and scale it appropriately. */

	emit2(B16(11100101,00000000) | (rd<<0));
	emit4(expr->val - pc);
}

