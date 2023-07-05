#include "mcg.h"

static struct basicblock* current_bb;

static int stackptr;
static struct ir* stack[64];

struct jumptable
{
	struct basicblock* defaulttarget;
	IMAPOF(struct basicblock) targets;
};

static struct ir*
convert(struct ir* src, int srcsize, int destsize, int opcode);
static struct ir* appendir(struct ir* ir);
static void insn_ivalue(int opcode, arith value);

static void parse_csa(struct basicblock* data_bb, struct jumptable* table);
static void parse_csb(struct basicblock* data_bb, struct jumptable* table);
static void emit_jumptable(struct ir* targetvalue, struct jumptable* table);

static void reset_stack(void)
{
	stackptr = 0;
}

static void push(struct ir* ir)
{
	if (stackptr == sizeof(stack) / sizeof(*stack))
		fatal("stack overflow");

#if 0
    /* If we try to push something which is too small, convert it to a word
     * first. */

    if (ir->size < EM_wordsize)
        ir = convertu(ir, EM_wordsize);
#endif

	stack[stackptr++] = ir;
}

/* Returns the size of the top item on the stack. */
static int peek(int delta)
{
	if (stackptr <= delta)
		return EM_wordsize;
	else
	{
		struct ir* ir = stack[stackptr - 1 - delta];
		return ir->size;
	}
}

static struct ir* pop(int size)
{
	if (size < EM_wordsize)
		size = EM_wordsize;

	if (stackptr == 0)
	{
		/* Nothing in our fake stack, so we have to read from the real stack. */

		if (size < EM_wordsize)
			size = EM_wordsize;
		return appendir(new_ir0(IR_POP, size));
	}
	else
	{
		struct ir* ir = stack[--stackptr];

#if 0
        /* If we try to pop something which is smaller than a word, convert it first. */

        if (size < EM_wordsize)
            ir = convertu(ir, size);
#endif
		if (ir->size != size)
		{
			if ((size == (EM_wordsize * 2)) && (ir->size == EM_wordsize))
			{
				/* Tried to read a long, but we got an int. Assemble the long
				 * out of two ints. Note that EM doesn't specify an order. */
				return new_ir2(IR_FROMIPAIR, size, ir, pop(EM_wordsize));
			}
			else if ((size == EM_wordsize) && (ir->size == (EM_wordsize * 2)))
			{
				appendir(ir);

				/* Tried to read an int, but we got a long. */
				push(new_ir1(IR_FROML1, EM_wordsize, ir));

				return new_ir1(IR_FROML0, EM_wordsize, ir);
			}
			else
				fatal(
				    "expected an item on stack of size %d, but got %d\n", size,
				    ir->size);
		}
		return ir;
	}
}

static void print_stack(void)
{
	int i;

	tracef('E', "E: stack:");
	for (i = 0; i < stackptr; i++)
	{
		struct ir* ir = stack[i];
		tracef('E', " $%d.%d", ir->id, ir->size);
	}
	tracef('E', "  (top)\n");
}

static struct ir* appendir(struct ir* ir)
{
	int i;

	assert(current_bb != NULL);
	array_appendu(&current_bb->irs, ir);

	ir_print('0', ir);
	return ir;
}

static void sequence_point(void)
{
	int i;

	/* Ensures that any partially-evaluated expressions on the stack are
	 * executed right now. This typically needs to happen before store
	 * operations, to prevents loads of the same address being delayed until
	 * after the store (at which point they'll return incorrect values).
	 */

	assert(current_bb != NULL);

	for (i = 0; i < stackptr; i++)
	{
		struct ir* ir = stack[i];
		array_appendu(&current_bb->irs, ir);
	}
}

static void materialise_stack(void)
{
	int i;

	for (i = 0; i < stackptr; i++)
	{
		struct ir* ir = stack[i];
		appendir(new_ir1(IR_PUSH, ir->size, ir));
	}

	reset_stack();
}

void tb_filestart(void)
{
}

void tb_fileend(void)
{
}

void tb_regvar(
    struct procedure* procedure, arith offset, int size, int type, int priority)
{
	struct local* local = calloc(1, sizeof(*local));
	local->size = size;
	local->offset = offset;
	local->is_register = true;
	imap_put(&procedure->locals, offset, local);
}

static struct ir* address_of_external(const char* label, arith offset)
{
	if (offset != 0)
		return new_ir2(
		    IR_ADD, EM_pointersize, new_labelir(label), new_wordir(offset));
	else
		return new_labelir(label);
}

static struct ir* convert(struct ir* src, int srcsize, int destsize, int opcode)
{
	if (srcsize == 1)
	{
		if ((opcode == IR_FROMSI) || (opcode == IR_FROMSL))
		{
			src = new_ir1(IR_EXTENDB, EM_wordsize, src);
		}
		srcsize = EM_wordsize;
	}

	if ((srcsize == 2) && (srcsize != EM_wordsize))
	{
		if ((opcode == IR_FROMSI) || (opcode == IR_FROMSL))
		{
			src = new_ir1(IR_EXTENDH, EM_wordsize, src);
		}
		srcsize = EM_wordsize;
	}

	if (src->size == EM_wordsize)
	{
	}
	else if (src->size == (2 * EM_wordsize))
		opcode++;
	else
		fatal("can't convert from %d to %d", src->size, destsize);

	return new_ir1(opcode, destsize, src);
}

static struct ir*
compare(struct ir* left, struct ir* right, int size, int opcode)
{
	if (size == EM_wordsize)
	{
	}
	else if (size == (2 * EM_wordsize))
		opcode++;
	else
		fatal(
		    "can't compare things of size %d; the word size is %d", size,
		    EM_wordsize);

	return new_ir2(opcode, EM_wordsize, left, right);
}

static struct ir*
store(int size, struct ir* address, int offset, struct ir* value)
{
	int opcode;

	sequence_point();

	if (size == 1)
	{
		opcode = IR_STOREB;
		size = EM_wordsize;
	}
	else if ((size < EM_wordsize) && (size == 2))
	{
		opcode = IR_STOREH;
		size = EM_wordsize;
	}
	else
		opcode = IR_STORE;

	if (offset != 0)
		address = new_ir2(IR_ADD, EM_pointersize, address, new_wordir(offset));

	return new_ir2(opcode, size, address, value);
}

static struct ir* load(int size, struct ir* address, int offset)
{
	int opcode;

	if (size == 1)
	{
		opcode = IR_LOADB;
		size = EM_wordsize;
	}
	else if ((size < EM_wordsize) && (size == 2))
	{
		opcode = IR_LOADH;
		size = EM_wordsize;
	}
	else
		opcode = IR_LOAD;

	if (offset != 0)
		address = new_ir2(IR_ADD, EM_pointersize, address, new_wordir(offset));

	return new_ir1(opcode, size, address);
}

static struct ir* tristate_compare(int size, int opcode)
{
	struct ir* right = pop(size);
	struct ir* left = pop(size);

	return compare(left, right, size, opcode);
}

static struct ir* tristate_compare0(int size, int opcode)
{
	struct ir* right = new_wordir(0);
	struct ir* left = pop(size);

	return compare(left, right, size, opcode);
}

static void simple_convert(int opcode)
{
	struct ir* destsize = pop(EM_wordsize);
	struct ir* srcsize = pop(EM_wordsize);
	struct ir* value;

	assert(srcsize->opcode == IR_CONST);
	assert(destsize->opcode == IR_CONST);

	value = pop(srcsize->u.ivalue);
	push(convert(value, srcsize->u.ivalue, destsize->u.ivalue, opcode));
}

static void simple_branch2(
    int opcode,
    int size,
    struct basicblock* truebb,
    struct basicblock* falsebb,
    int irop)
{
	struct ir* right = pop(size);
	struct ir* left = pop(size);

	materialise_stack();
	appendir(new_ir2(
	    irop, 0, compare(left, right, size, IR_COMPARESI),
	    new_ir2(IR_PAIR, 0, new_bbir(truebb), new_bbir(falsebb))));
}

static void compare0_branch2(
    int opcode, struct basicblock* truebb, struct basicblock* falsebb, int irop)
{
	push(new_wordir(0));

	simple_branch2(opcode, EM_wordsize, truebb, falsebb, irop);
}

static void simple_test(int size, int irop)
{
	push(new_ir1(irop, EM_wordsize, tristate_compare0(size, IR_COMPARESI)));
}

static void simple_test_neg(int size, int irop)
{
	simple_test(size, irop);

	push(new_ir1(IR_NOT, EM_wordsize, pop(EM_wordsize)));
}

static void helper_function(const char* name)
{
	/* Delegates to a helper function; these leave their result on the stack
	 * rather than returning values through lfr. */

	materialise_stack();
	appendir(new_ir1(IR_CALL, 0, new_labelir(name)));
}

static void helper_function_with_arg(const char* name, struct ir* arg)
{
	/* Abuses IR_SETRET to set a register to pass one argument to a
	 * helper function.
	 *
	 * FIXME:  As of January 2018, mach/powerpc/libem takes an
	 * argument in register r3 only for ".los4", ".sts4", ".trp".
	 * This is an accident.  Should the argument be on the stack, or
	 * should other helpers use a register? */

	materialise_stack();
	appendir(new_ir1(IR_SETRET, arg->size, arg));
	appendir(new_ir1(IR_CALL, 0, new_labelir(name)));
}

static void insn_simple(int opcode)
{
	switch (opcode)
	{
		case op_bra:
		{
			struct ir* dest = pop(EM_pointersize);

			materialise_stack();
			appendir(new_ir1(IR_JUMP, 0, dest));
			break;
		}

		case op_cii:
			simple_convert(IR_FROMSI);
			break;
		case op_ciu:
			simple_convert(IR_FROMSI);
			break;
		case op_cui:
			simple_convert(IR_FROMUI);
			break;
		case op_cuu:
			simple_convert(IR_FROMUI);
			break;
		case op_cfu:
			simple_convert(IR_FROMUF);
			break;
		case op_cfi:
			simple_convert(IR_FROMSF);
			break;
		case op_cif:
			simple_convert(IR_FROMSI);
			break;
		case op_cuf:
			simple_convert(IR_FROMUI);
			break;
		case op_cff:
			simple_convert(IR_FROMSF);
			break;

		case op_cmp:
			push(tristate_compare(EM_pointersize, IR_COMPAREUI));
			break;

		case op_teq:
			simple_test(EM_wordsize, IR_IFEQ);
			break;
		case op_tne:
			simple_test_neg(EM_wordsize, IR_IFEQ);
			break;
		case op_tlt:
			simple_test(EM_wordsize, IR_IFLT);
			break;
		case op_tge:
			simple_test_neg(EM_wordsize, IR_IFLT);
			break;
		case op_tle:
			simple_test(EM_wordsize, IR_IFLE);
			break;
		case op_tgt:
			simple_test_neg(EM_wordsize, IR_IFLE);
			break;

		case op_cai:
		{
			struct ir* dest = pop(EM_pointersize);

			materialise_stack();
			appendir(new_ir1(IR_CALL, 0, dest));
			break;
		}

		case op_inc:
		{
			push(new_ir2(IR_ADD, EM_wordsize, pop(EM_wordsize), new_wordir(1)));
			break;
		}

		case op_dec:
		{
			push(new_ir2(IR_SUB, EM_wordsize, pop(EM_wordsize), new_wordir(1)));
			break;
		}

		case op_lim:
		{
			/* Traps use only 16 bits of .ignmask, but we keep an
			 * entire word, even if a word has more than 2 bytes. */
			push(load(EM_wordsize, new_labelir(".ignmask"), 0));
			break;
		}

		case op_sim:
		{
			appendir(store(
			    EM_wordsize, new_labelir(".ignmask"), 0, pop(EM_wordsize)));
			break;
		}

		case op_trp:
			helper_function_with_arg(".trp", pop(EM_wordsize));
			break;

		case op_sig:
		{
			struct ir* label = new_labelir(".trppc");
			struct ir* value = pop(EM_pointersize);
			appendir(label); /* because we need to use label twice */
			push(load(EM_pointersize, label, 0));
			appendir(store(EM_pointersize, label, 0, value));
			break;
		}

		case op_rtt:
		{
			insn_ivalue(op_ret, 0);
			break;
		}

		case op_and:
			helper_function(".and");
			break;
		case op_ior:
			helper_function(".ior");
			break;
		case op_xor:
			helper_function(".xor");
			break;
		case op_com:
			helper_function(".com");
			break;
		case op_cms:
			helper_function(".cms");
			break;
		case op_set:
			helper_function(".set");
			break;
		case op_inn:
			helper_function(".inn");
			break;

		case op_dch:
			push(new_ir1(IR_CHAINFP, EM_pointersize, pop(EM_pointersize)));
			break;

		case op_lpb:
			push(new_ir1(IR_FPTOAB, EM_pointersize, pop(EM_pointersize)));
			break;

		case op_lni:
		{
			/* Increment line number --- ignore. */
			break;
		}

		default:
			fatal(
			    "treebuilder: unknown simple instruction '%s'",
			    em_mnem[opcode - sp_fmnem]);
	}
}

static void
insn_bvalue(int opcode, struct basicblock* leftbb, struct basicblock* rightbb)
{
	switch (opcode)
	{
		case op_zeq:
			compare0_branch2(opcode, leftbb, rightbb, IR_CJUMPEQ);
			break;
		case op_zlt:
			compare0_branch2(opcode, leftbb, rightbb, IR_CJUMPLT);
			break;
		case op_zle:
			compare0_branch2(opcode, leftbb, rightbb, IR_CJUMPLE);
			break;

		case op_zne:
			compare0_branch2(opcode, rightbb, leftbb, IR_CJUMPEQ);
			break;
		case op_zge:
			compare0_branch2(opcode, rightbb, leftbb, IR_CJUMPLT);
			break;
		case op_zgt:
			compare0_branch2(opcode, rightbb, leftbb, IR_CJUMPLE);
			break;

		case op_beq:
			simple_branch2(opcode, EM_wordsize, leftbb, rightbb, IR_CJUMPEQ);
			break;
		case op_blt:
			simple_branch2(opcode, EM_wordsize, leftbb, rightbb, IR_CJUMPLT);
			break;
		case op_ble:
			simple_branch2(opcode, EM_wordsize, leftbb, rightbb, IR_CJUMPLE);
			break;

		case op_bne:
			simple_branch2(opcode, EM_wordsize, rightbb, leftbb, IR_CJUMPEQ);
			break;
		case op_bge:
			simple_branch2(opcode, EM_wordsize, rightbb, leftbb, IR_CJUMPLT);
			break;
		case op_bgt:
			simple_branch2(opcode, EM_wordsize, rightbb, leftbb, IR_CJUMPLE);
			break;

		case op_bra:
		{
			materialise_stack();

			appendir(new_ir1(IR_JUMP, 0, new_bbir(leftbb)));
			break;
		}

		case op_lae:
			push(new_bbir(leftbb));
			break;

		default:
			fatal(
			    "treebuilder: unknown bvalue instruction '%s'",
			    em_mnem[opcode - sp_fmnem]);
	}
}

static void simple_alu1(int opcode, int size, int irop, const char* fallback)
{
	if (size > (2 * EM_wordsize))
	{
		if (!fallback)
			fatal(
			    "treebuilder: can't do opcode %s with size %d",
			    em_mnem[opcode - sp_fmnem], size);
		push(new_wordir(size));
		helper_function(fallback);
	}
	else
	{
		struct ir* val = pop(size);

		push(new_ir1(irop, size, val));
	}
}

static void simple_alu2(int opcode, int size, int irop, const char* fallback)
{
	if (size > (2 * EM_wordsize))
	{
		if (!fallback)
			fatal(
			    "treebuilder: can't do opcode %s with size %d",
			    em_mnem[opcode - sp_fmnem], size);
		push(new_wordir(size));
		helper_function(fallback);
	}
	else
	{
		struct ir* right = pop(size);
		struct ir* left = pop(size);

		push(new_ir2(irop, size, left, right));
	}
}

static void rotate(int opcode, int size, int irop, int irop_reverse)
{
	if (size > (2 * EM_wordsize))
		fatal(
		    "treebuilder: can't do opcode %s with size %d",
		    em_mnem[opcode - sp_fmnem], size);
	else
	{
		struct ir* right = pop(size);
		struct ir* left = pop(size);
		struct ir* bits = new_wordir(8 * size);

		/* Fix left and right so we can refer to them multiple times. */
		appendir(right);
		appendir(left);

		/* a rol b -> (a << b) | (a >> (32 - b)) */
		push(new_ir2(
		    IR_OR, size, new_ir2(irop, size, left, right),
		    new_ir2(
		        irop_reverse, size, left, new_ir2(IR_SUB, size, bits, right))));
	}
}

static void change_by(struct ir* address, int amount)
{
	appendir(store(
	    EM_wordsize, address, 0,
	    new_ir2(
	        IR_ADD, EM_wordsize, load(EM_wordsize, address, 0),
	        new_wordir(amount))));
}

static struct ir* ptradd(struct ir* address, int offset)
{
	if (offset == 0)
		return address;

	return new_ir2(IR_ADD, EM_pointersize, address, new_wordir(offset));
}

static struct ir* walk_static_chain(int level)
{
	struct ir* ir;

	/* The static chain, when it exists, is the first argument of each
	 * procedure.  The chain begins with the current frame at level 0,
	 * and continues until we reach the outermost procedure. */
	ir = new_ir0(IR_GETFP, EM_pointersize);
	while (level--)
	{
		/* Walk to the next frame pointer. */
		ir = load(EM_pointersize, new_ir1(IR_FPTOAB, EM_pointersize, ir), 0);
	}
	return ir;
}

static void insn_ivalue(int opcode, arith value)
{
	switch (opcode)
	{
		case op_adi:
			simple_alu2(opcode, value, IR_ADD, NULL);
			break;
		case op_sbi:
			simple_alu2(opcode, value, IR_SUB, NULL);
			break;
		case op_mli:
			simple_alu2(opcode, value, IR_MUL, NULL);
			break;
		case op_dvi:
			simple_alu2(opcode, value, IR_DIV, NULL);
			break;
		case op_rmi:
			simple_alu2(opcode, value, IR_MOD, NULL);
			break;
		case op_sli:
			simple_alu2(opcode, value, IR_LSL, NULL);
			break;
		case op_sri:
			simple_alu2(opcode, value, IR_ASR, NULL);
			break;
		case op_ngi:
			simple_alu1(opcode, value, IR_NEG, NULL);
			break;

		case op_adu:
			simple_alu2(opcode, value, IR_ADD, NULL);
			break;
		case op_sbu:
			simple_alu2(opcode, value, IR_SUB, NULL);
			break;
		case op_mlu:
			simple_alu2(opcode, value, IR_MUL, NULL);
			break;
		case op_slu:
			simple_alu2(opcode, value, IR_LSL, NULL);
			break;
		case op_sru:
			simple_alu2(opcode, value, IR_LSR, NULL);
			break;
		case op_rmu:
			simple_alu2(opcode, value, IR_MODU, NULL);
			break;
		case op_dvu:
			simple_alu2(opcode, value, IR_DIVU, NULL);
			break;

		case op_and:
			simple_alu2(opcode, value, IR_AND, ".and");
			break;
		case op_ior:
			simple_alu2(opcode, value, IR_OR, ".ior");
			break;
		case op_xor:
			simple_alu2(opcode, value, IR_EOR, ".xor");
			break;
		case op_com:
			simple_alu1(opcode, value, IR_NOT, ".com");
			break;
		case op_rol:
			rotate(opcode, value, IR_LSL, IR_LSR);
			break;
		case op_ror:
			rotate(opcode, value, IR_LSR, IR_LSL);
			break;

		case op_adf:
			simple_alu2(opcode, value, IR_ADDF, NULL);
			break;
		case op_sbf:
			simple_alu2(opcode, value, IR_SUBF, NULL);
			break;
		case op_mlf:
			simple_alu2(opcode, value, IR_MULF, NULL);
			break;
		case op_dvf:
			simple_alu2(opcode, value, IR_DIVF, NULL);
			break;
		case op_ngf:
			simple_alu1(opcode, value, IR_NEGF, NULL);
			break;

		case op_cms:
			if (value > (2 * EM_wordsize))
			{
				push(new_wordir(value));
				helper_function(".cms");
				break;
			}
			/* fall through */
		case op_cmu:
			push(tristate_compare(value, IR_COMPAREUI));
			break;
		case op_cmi:
			push(tristate_compare(value, IR_COMPARESI));
			break;
		case op_cmf:
			push(tristate_compare(value, IR_COMPAREF));
			break;

		case op_rck:
			if (value != EM_wordsize)
				fatal("'rck %d' not supported", value);
            #ifdef MCGG_OPTION_NO_BOUNDS_CHECKING
                pop(EM_pointersize);
            #else
                helper_function(".rck");
            #endif
			break;
		case op_set:
			push(new_wordir(value));
			helper_function(".set");
			break;
		case op_inn:
			push(new_wordir(value));
			helper_function(".inn");
			break;

		case op_lol:
			push(load(EM_wordsize, new_localir(value), 0));
			break;

		case op_ldl:
			push(load(EM_wordsize * 2, new_localir(value), 0));
			break;

		case op_stl:
			appendir(
			    store(EM_wordsize, new_localir(value), 0, pop(EM_wordsize)));
			break;

		case op_sdl:
			appendir(store(
			    EM_wordsize * 2, new_localir(value), 0, pop(EM_wordsize * 2)));
			break;

		case op_lal:
			push(new_localir(value));
			break;

		case op_lil:
			push(load(
			    EM_wordsize, load(EM_pointersize, new_localir(value), 0), 0));
			break;

		case op_sil:
			appendir(store(
			    EM_wordsize, load(EM_pointersize, new_localir(value), 0), 0,
			    pop(EM_wordsize)));
			break;

		case op_inl:
			change_by(new_localir(value), 1);
			break;

		case op_del:
			change_by(new_localir(value), -1);
			break;

		case op_zrl:
			appendir(store(EM_wordsize, new_localir(value), 0, new_wordir(0)));
			break;

		case op_zrf:
		{
			struct ir* ir = new_constir(value, 0);
			ir->opcode = IR_CONST;
			push(ir);
			break;
		}

		case op_loe:
			push(load(EM_wordsize, new_labelir(".hol0"), value));
			break;

		case op_lae:
			push(new_ir2(
			    IR_ADD, EM_pointersize, new_labelir(".hol0"),
			    new_wordir(value)));
			break;

		case op_ste:
			appendir(store(
			    EM_wordsize, new_labelir(".hol0"), value, pop(EM_wordsize)));
			break;

		case op_zre:
			appendir(
			    store(EM_wordsize, new_labelir(".hol0"), value, new_wordir(0)));
			break;

		case op_loc:
			push(new_wordir(value));
			break;

		case op_ldc:
			push(new_longir(value));
			break;

		case op_loi:
		{
			struct ir* ptr = pop(EM_pointersize);
			int offset = 0;

			if (value > (EM_wordsize * 2))
			{
				/* We're going to need to do multiple loads; fix the address
				 * so it'll go into a register and we can do maths on it. */
				appendir(ptr);
			}

			/* Stack grows down.  Load backwards. */
			while (value > 0)
			{
				int s = EM_wordsize * 2;
				if (value < s)
					s = value;
				value -= s;
				push(load(s, ptr, value));
			}

			assert(value == 0);
			break;
		}

		case op_lof:
		{
			struct ir* ptr = pop(EM_pointersize);

			push(load(EM_wordsize, ptr, value));
			break;
		}

		case op_ldf:
		{
			struct ir* ptr = pop(EM_pointersize);

			push(load(EM_wordsize * 2, ptr, value));
			break;
		}

		case op_sti:
		{
			struct ir* ptr = pop(EM_pointersize);
			int offset = 0;

			if (value > peek(0))
			{
				/* We're going to need to do multiple stores; fix the address
				 * so it'll go into a register and we can do maths on it. */
				appendir(ptr);
			}

			while (value > 0)
			{
				struct ir* v = pop(peek(0));
				int s = v->size;
				if (value < s)
					s = value;

				appendir(store(s, ptr, offset, v));

				value -= s;
				offset += s;
			}

			assert(value == 0);
			break;
		}

		case op_stf:
		{
			struct ir* ptr = pop(EM_pointersize);
			struct ir* val = pop(EM_wordsize);

			appendir(store(EM_wordsize, ptr, value, val));
			break;
		}

		case op_sdf:
		{
			struct ir* ptr = pop(EM_pointersize);
			struct ir* val = pop(EM_wordsize * 2);

			appendir(store(EM_wordsize * 2, ptr, value, val));
			break;
		}

		case op_ads:
		{
			struct ir* off = pop(value);
			struct ir* ptr = pop(EM_pointersize);

			if (value != EM_pointersize)
				off = convert(off, value, EM_pointersize, IR_FROMUI);

			push(new_ir2(IR_ADD, EM_pointersize, ptr, off));
			break;
		}

		case op_adp:
		{
			struct ir* ptr = pop(EM_pointersize);

			push(new_ir2(IR_ADD, EM_pointersize, ptr, new_wordir(value)));
			break;
		}

		case op_sbs:
		{
			struct ir* right = pop(EM_pointersize);
			struct ir* left = pop(EM_pointersize);

			struct ir* delta = new_ir2(IR_SUB, EM_pointersize, left, right);

			if (value != EM_pointersize)
				delta = convert(delta, EM_pointersize, value, IR_FROMUI);

			push(delta);
			break;
		}

		case op_dup:
		{
			sequence_point();
			if (value > (2 * EM_wordsize))
			{
				push(new_wordir(value));
				helper_function(".dus");
			}
			else if (
			    (value == (EM_wordsize * 2)) && (peek(0) == EM_wordsize)
			    && (peek(1) == EM_wordsize))
			{
				struct ir* v1 = pop(EM_wordsize);
				struct ir* v2 = pop(EM_wordsize);
				push(v2);
				push(v1);
				push(v2);
				push(v1);
			}
			else
			{
				struct ir* v = pop(value);
				push(v);
				push(v);
			}
			break;
		}

		case op_dus:
		{
			if (value != EM_wordsize)
				fatal("'dus %d' not supported", value);
			helper_function(".dus");
			break;
		}

		case op_exg:
		{
			if (value > (2 * EM_wordsize))
			{
				push(new_wordir(value));
				helper_function(".exg");
			}
			else
			{
				struct ir* v1 = pop(value);
				struct ir* v2 = pop(value);
				push(v1);
				push(v2);
			}
			break;
		}

		case op_zer:
		{
			if (value <= EM_wordsize)
				push(new_constir(value, 0));
			else
			{
				push(new_wordir(value));
				helper_function(".zer");
			}
			break;
		}

		case op_asp:
		{
			switch (value)
			{
				case 0:
					break;

				case -1:
				case -2:
				case -4:
				case -8:
					push(new_anyir(-value));
					break;

				default:
					while ((value > 0) && (stackptr > 0))
					{
						int s = peek(0);
						if (s > value)
							s = value;
						pop(s);
						value -= s;
					}

					appendir(new_ir1(
					    IR_STACKADJUST, EM_pointersize, new_wordir(value)));
					break;
			}
			break;
		}

		case op_ass:
			appendir(new_ir1(IR_STACKADJUST, EM_pointersize, pop(value)));
			break;

		case op_ret:
		{
			if (value > 0)
			{
				struct ir* retval = pop(value);
				materialise_stack();
				appendir(new_ir1(IR_SETRET, value, retval));
			}

			if (!current_proc->exit)
			{
				current_proc->exit = bb_get(NULL);
				array_append(&current_proc->blocks, current_proc->exit);

				/* This is actually ignored --- the entire block gets special
				 * treatment. But a lot of the rest of the code assumes that
				 * all basic blocks have one instruction, so we insert one. */

				array_append(&current_proc->exit->irs, new_ir0(IR_RET, 0));
			}

			appendir(new_ir1(IR_JUMP, 0, new_bbir(current_proc->exit)));
			break;
		}

		case op_lfr:
		{
			push(appendir(new_ir0(IR_GETRET, value)));
			break;
		}

		case op_csa:
		{
			struct ir* descriptor = pop(EM_pointersize);
			struct ir* targetvalue = appendir(pop(EM_pointersize));
			struct jumptable jumptable = {};
			int i;

			if (descriptor->opcode != IR_LABEL)
				fatal("csa is only supported if it refers "
				      "directly to a descriptor block");

			parse_csa(bb_get(descriptor->u.lvalue), &jumptable);
			emit_jumptable(targetvalue, &jumptable);
			break;
		}

		case op_csb:
		{
			struct ir* descriptor = pop(EM_pointersize);
			struct ir* targetvalue = appendir(pop(EM_pointersize));
			struct jumptable jumptable = {};
			int i;

			if (descriptor->opcode != IR_LABEL)
				fatal("csb is only supported if it refers "
				      "directly to a descriptor block");

			parse_csb(bb_get(descriptor->u.lvalue), &jumptable);
			emit_jumptable(targetvalue, &jumptable);
			break;
		}

		case op_sar:
		case op_lar:
		case op_aar:
		{
			const char* helper = NULL;
			if (value != EM_wordsize)
				fatal("sar/lar/aar are only supported when using "
				      "word-size descriptors");

            #if defined MCGG_OPTION_NO_BOUNDS_CHECKING
            /* The stack contains ( array-ptr index descriptor-ptr ) at this point. We use the same code for all options to calculate the address. */
            {
                struct ir* descriptor = pop(EM_pointersize);
                struct ir* index = pop(EM_wordsize);
                struct ir* arrayptr = pop(EM_pointersize);
                struct ir* elementsize = load(EM_wordsize, descriptor, EM_wordsize*2);
                struct ir* baseindex = load(EM_wordsize, descriptor, 0);

                appendir(arrayptr);
                appendir(elementsize);
                struct ir* address =
                    new_ir2(
                        IR_ADD, EM_pointersize,
                        arrayptr,
                        convert(
                            new_ir2(
                                IR_MUL, EM_wordsize,
                                new_ir2(
                                    IR_SUB, EM_wordsize,
                                    index,
                                    baseindex),
                                elementsize),
                            EM_wordsize, EM_pointersize, IR_FROMUI));
                appendir(address);

                switch (opcode)
                {
                    case op_aar:
                        push(address);
                        break;
                    case op_lar:
                        push(address);
                        push(elementsize);
                        helper = ".loi";
                        break;
                    case op_sar:
                        push(address);
                        push(elementsize);
                        helper = ".sti";
                        break;
                }
            }
            #else
                switch (opcode)
                {
                    case op_sar:
                        helper = ".sar";
                        break;
                    case op_lar:
                        helper = ".lar";
                        break;
                    case op_aar:
                        helper = ".aar";
                        break;
                }
            #endif

            if (helper)
            {
                materialise_stack();
                /* No push here, because the helper function leaves the result on
                 * the physical stack (which is very dubious). */
                appendir(new_ir1(IR_CALL, 0, new_labelir(helper)));
            }
			break;
		}

		case op_lxl:
			push(walk_static_chain(value));
			break;

		case op_lxa:
			push(new_ir1(IR_FPTOAB, EM_pointersize, walk_static_chain(value)));
			break;

		case op_fef:
		{
			struct ir* f = pop(value);

			/* fef is implemented by calling a helper function which then
			 * mutates the stack. We read the return values off the stack when
			 * retracting the stack pointer. */

			push(f);
			push(new_wordir(0));

			materialise_stack();
			appendir(new_ir1(
			    IR_CALL, 0, new_labelir(
                    aprintf(".fef%d", value))));

			/* exit, leaving an int and then a float (or double) on the stack.
			 */
			break;
		}

		case op_fif:
		{
			/* fif is implemented by calling a helper function which then
			 * mutates the stack. We read the return values off the stack when
			 * retracting the stack pointer. */

			/* We start with two floats on the stack. */

			materialise_stack();
			appendir(new_ir1(
			    IR_CALL, 0, new_labelir(
                    aprintf(".fif%d", value))));

			/* exit, leaving two floats (or doubles) on the stack. */
			break;
		}

		case op_lor:
		{
			switch (value)
			{
				case 0:
					push(appendir(new_ir1(
					    IR_FPTOLB, EM_pointersize,
					    new_ir0(IR_GETFP, EM_pointersize))));
					break;

				case 1:
					materialise_stack();
					push(appendir(new_ir0(IR_GETSP, EM_pointersize)));
					break;

				default:
					fatal("'lor %d' not supported", value);
			}

			break;
		}

		case op_str:
		{
			switch (value)
			{
				case 0:
					appendir(
					    new_ir1(IR_SETFP, EM_pointersize, pop(EM_pointersize)));
					break;

				case 1:
					appendir(
					    new_ir1(IR_SETSP, EM_pointersize, pop(EM_pointersize)));
					break;

				default:
					fatal("'str %d' not supported", value);
			}

			break;
		}

		case op_blm:
			push(new_wordir(value));
			helper_function(".bls");
			break;

		case op_bls:
			if (value != EM_wordsize)
				fatal("'bls %d' not supported", value);
			helper_function(".bls");
			break;

		case op_los:
			if (value != EM_wordsize)
				fatal("'los %d' not supported", value);
			helper_function_with_arg(".los", pop(EM_wordsize));
			break;

		case op_sts:
			if (value != EM_wordsize)
				fatal("'sts %d' not supported", value);
			helper_function_with_arg(".sts", pop(EM_wordsize));
			break;

		case op_lin:
		{
			/* Set line number --- ignore. */
			break;
		}

		default:
			fatal(
			    "treebuilder: unknown ivalue instruction '%s'",
			    em_mnem[opcode - sp_fmnem]);
	}
}

static void insn_lvalue(int opcode, const char* label, arith offset)
{
	switch (opcode)
	{
		case op_lpi:
		case op_lae:
			push(address_of_external(label, offset));
			break;

		case op_loe:
			push(new_ir1(
			    IR_LOAD, EM_wordsize, address_of_external(label, offset)));
			break;

		case op_lde:
			push(new_ir1(
			    IR_LOAD, EM_wordsize * 2, address_of_external(label, offset)));
			break;

		case op_ste:
			sequence_point();
			appendir(new_ir2(
			    IR_STORE, EM_wordsize, address_of_external(label, offset),
			    pop(EM_wordsize)));
			break;

		case op_sde:
			sequence_point();
			appendir(new_ir2(
			    IR_STORE, EM_wordsize * 2, address_of_external(label, offset),
			    pop(EM_wordsize * 2)));
			break;

		case op_zre:
			sequence_point();
			appendir(new_ir2(
			    IR_STORE, EM_wordsize, address_of_external(label, offset),
			    new_wordir(0)));
			break;

		case op_ine:
			sequence_point();
			appendir(new_ir2(
			    IR_STORE, EM_wordsize, address_of_external(label, offset),
			    new_ir2(
			        IR_ADD, EM_wordsize,
			        new_ir1(
			            IR_LOAD, EM_wordsize,
			            address_of_external(label, offset)),
			        new_wordir(1))));
			break;

		case op_dee:
			sequence_point();
			appendir(new_ir2(
			    IR_STORE, EM_wordsize, address_of_external(label, offset),
			    new_ir2(
			        IR_ADD, EM_wordsize,
			        new_ir1(
			            IR_LOAD, EM_wordsize,
			            address_of_external(label, offset)),
			        new_wordir(-1))));
			break;

		case op_cal:
			assert(offset == 0);
			materialise_stack();
			appendir(new_ir1(IR_CALL, 0, new_labelir(label)));
			break;

		case op_bra:
			assert(offset == 0);
			materialise_stack();
			appendir(new_ir1(IR_JUMP, 0, new_labelir(label)));
			break;

		case op_gto:
		{
			struct ir* descriptor = address_of_external(label, offset);

			appendir(new_ir1(
			    IR_SETFP, EM_pointersize,
			    load(EM_pointersize, descriptor, EM_pointersize * 2)));
			appendir(new_ir1(
			    IR_SETSP, EM_pointersize,
			    load(EM_pointersize, descriptor, EM_pointersize * 1)));
			appendir(new_ir1(
			    IR_JUMP, 0,
			    load(EM_pointersize, descriptor, EM_pointersize * 0)));
			break;
		}

		case op_fil:
		{
			/* Set filename --- ignore. */
			break;
		}

		default:
			fatal(
			    "treebuilder: unknown lvalue instruction '%s'",
			    em_mnem[opcode - sp_fmnem]);
	}
}

static void generate_tree(struct basicblock* bb)
{
	int i;

	tracef('0', "0: block %s\n", bb->name);

	current_bb = bb;
	reset_stack();

	for (i = 0; i < bb->ems.count; i++)
	{
		struct em* em = bb->ems.item[i];
		tracef('E', "E: read %s ", em_mnem[em->opcode - sp_fmnem]);
		switch (em->paramtype)
		{
			case PARAM_NONE:
				tracef('E', "\n");
				insn_simple(em->opcode);
				break;

			case PARAM_IVALUE:
				tracef('E', "value=%d\n", em->u.ivalue);
				insn_ivalue(em->opcode, em->u.ivalue);
				break;

			case PARAM_LVALUE:
				tracef(
				    'E', "label=%s offset=%d\n", em->u.lvalue.label,
				    em->u.lvalue.offset);
				insn_lvalue(
				    em->opcode, em->u.lvalue.label, em->u.lvalue.offset);
				break;

			case PARAM_BVALUE:
				tracef('E', "true=%s", em->u.bvalue.left->name);
				if (em->u.bvalue.right)
					tracef('E', " false=%s", em->u.bvalue.right->name);
				tracef('E', "\n");
				insn_bvalue(em->opcode, em->u.bvalue.left, em->u.bvalue.right);
				break;

			default:
				assert(0);
		}

		if (tracing('E'))
			print_stack();
	}

	/* Yes, we are allowed to leave stuff on the stack at the end of the
	 * procedure. It's discarded as part of the function return. */
}

void tb_procedure(void)
{
	int i;

	for (i = 0; i < current_proc->blocks.count; i++)
		generate_tree(current_proc->blocks.item[i]);
}

static void parse_csa(struct basicblock* data_bb, struct jumptable* table)
{
	struct em* em;
	int lowerbound;
	int count;
	int i;

	assert(data_bb->ems.count >= 3);

	/* Default target */

	em = data_bb->ems.item[0];
	assert(em->opcode == op_bra);
	assert(em->paramtype == PARAM_BVALUE);
	table->defaulttarget = em->u.bvalue.left;

	/* Lower bound */

	em = data_bb->ems.item[1];
	assert(em->opcode == op_loc);
	assert(em->paramtype == PARAM_IVALUE);
	lowerbound = em->u.ivalue;

	/* Count */

	em = data_bb->ems.item[2];
	assert(em->opcode == op_loc);
	assert(em->paramtype == PARAM_IVALUE);
	count = em->u.ivalue + 1; /* value in descriptor is inclusive */
	assert(data_bb->ems.count >= (count + 3));

	/* Now, each target in turn. */

	for (i = 0; i < count; i++)
	{
		struct basicblock* target;

		em = data_bb->ems.item[3 + i];
		assert(em->opcode == op_bra);
		assert(em->paramtype == PARAM_BVALUE);
		target = em->u.bvalue.left;

		imap_put(&table->targets, lowerbound + i, target);
	}
}

static void parse_csb(struct basicblock* data_bb, struct jumptable* table)
{
	struct em* em;
	int count;
	int i;

	assert(data_bb->ems.count >= 2);

	/* Default target */

	em = data_bb->ems.item[0];
	assert(em->opcode == op_bra);
	assert(em->paramtype == PARAM_BVALUE);
	table->defaulttarget = em->u.bvalue.left;

	/* Number of targets */

	em = data_bb->ems.item[1];
	assert(em->opcode == op_loc);
	assert(em->paramtype == PARAM_IVALUE);
	count = em->u.ivalue;
	assert(data_bb->ems.count >= (count * 2 + 2));

	/* Now, each target in turn. */

	for (i = 0; i < count; i++)
	{
		int value;
		struct basicblock* target;

		em = data_bb->ems.item[2 + i * 2];
		assert(em->opcode == op_loc);
		assert(em->paramtype == PARAM_IVALUE);
		value = em->u.ivalue;

		em = data_bb->ems.item[3 + i * 2];
		assert(em->opcode == op_bra);
		assert(em->paramtype == PARAM_BVALUE);
		target = em->u.bvalue.left;

		imap_put(&table->targets, value, target);
	}
}

static void emit_jumptable(struct ir* targetvalue, struct jumptable* jumptable)
{
	int i;

	materialise_stack();
	for (i = 0; i < jumptable->targets.count; i++)
	{
		int value = jumptable->targets.item[i].left;
		struct basicblock* target = jumptable->targets.item[i].right;
		struct basicblock* nextblock = bb_get(NULL);

		array_append(&current_proc->blocks, nextblock);
		appendir(new_ir2(
		    IR_CJUMPEQ, 0,
		    new_ir2(IR_COMPARESI, EM_wordsize, targetvalue, new_wordir(value)),
		    new_ir2(IR_PAIR, 0, new_bbir(target), new_bbir(nextblock))));

		current_bb = nextblock;
	}

	appendir(new_ir1(IR_JUMP, 0, new_bbir(jumptable->defaulttarget)));
}

/* vim: set sw=4 ts=4 expandtab : */
