#include "mcg.h"

static struct basicblock* current_bb;

static int stackptr;
static struct ir* stack[64];

static struct ir* convert(struct ir* src, int destsize, int opcodebase);
static struct ir* appendir(struct ir* ir);

static void reset_stack(void)
{
    stackptr = 0;
}

static void push(struct ir* ir)
{
    if (stackptr == sizeof(stack)/sizeof(*stack))
        fatal("stack overflow");

    /* If we try to push something which is too small, convert it to a word
     * first. */

    if (ir->size < EM_wordsize)
        ir = convert(ir, EM_wordsize, IR_CIU1);

    stack[stackptr++] = ir;
}

static struct ir* pop(int size)
{
    if (stackptr == 0)
    {
        /* Nothing in our fake stack, so we have to read from the real stack. */

        if (size < EM_wordsize)
            size = EM_wordsize;
        return
            appendir(
                new_ir0(
                    IR_POP, size
                )
            );
    }
    else
    {
        struct ir* ir = stack[--stackptr];

        /* If we try to pop something which is smaller than a word, convert it first. */
        
        if (size < EM_wordsize)
            ir = convert(ir, size, IR_CIU1);

        if (ir->size != size)
            fatal("expected an item on stack of size %d, but got %d\n", size, ir->size);
        return ir;
    }
}

static void print_stack(void)
{
    int i;

    tracef('E', "E: stack:");
    for (i=0; i<stackptr; i++)
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
    array_append(&current_bb->irs, ir);

    ir_print('0', ir);
    return ir;
}

static void materialise_stack(void)
{
    int i;

    for (i=0; i<stackptr; i++)
    {
        struct ir* ir = stack[i];
        appendir(
            new_ir1(
                IR_PUSH, ir->size,
                ir
            )
        );
    }

    reset_stack();
}

void tb_filestart(void)
{
}

void tb_fileend(void)
{
}

void tb_regvar(struct procedure* procedure, arith offset, int size, int type, int priority)
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
        return
            new_ir2(
                IR_ADD, EM_pointersize,
                new_labelir(label),
                new_wordir(offset)
            );
    else
        return
            new_labelir(label);
}

static struct ir* convert(struct ir* src, int destsize, int opcode)
{
    switch (src->size)
    {
        case 1: opcode += 0; break;
        case 2: opcode += 1; break;
        case 4: opcode += 2; break;
        case 8: opcode += 3; break;
        default:
            fatal("can't convert from things of size %d", src->size);
    }

    return
        new_ir1(
            opcode, destsize,
            src
        );
}

static struct ir* tristate_compare(int size, int opcode)
{
    struct ir* right = pop(size);
    struct ir* left = pop(size);

    return
        new_ir2(
            opcode, EM_wordsize,
            left, right
        );
}

static void simple_convert(int opcode)
{
    struct ir* destsize = pop(EM_wordsize);
    struct ir* srcsize = pop(EM_wordsize);
    struct ir* value;

    assert(srcsize->opcode == IR_CONST);
    assert(destsize->opcode == IR_CONST);

    value = pop(srcsize->u.ivalue);
    push(
        convert(value, destsize->u.ivalue, opcode)
    );
}

static void insn_simple(int opcode)
{
    switch (opcode)
    {
        case op_bra:
        {
            struct ir* dest = pop(EM_pointersize);

            materialise_stack();
            appendir(
                new_ir1(
                    IR_JUMP, 0,
                    dest
                )
            );
            break;
        }
            
        case op_cii: simple_convert(IR_CII1); break;
        case op_ciu: simple_convert(IR_CIU1); break;

        case op_cmp:
            push(
                tristate_compare(EM_pointersize, IR_COMPAREU)
            );
            break;

        case op_cai:
        {
            struct ir* dest = pop(EM_pointersize);

            materialise_stack();
            appendir(
                new_ir1(
                    IR_CALL, 0,
                    dest
                )
            );
            break;
        }

        default:
            fatal("treebuilder: unknown simple instruction '%s'",
                em_mnem[opcode - sp_fmnem]);
    }
}

static void simple_branch2(int opcode, int size,
    struct basicblock* truebb, struct basicblock* falsebb,
    int irop)
{
    struct ir* right = pop(size);
    struct ir* left = pop(size);

    materialise_stack();
    appendir(
        new_ir2(
            irop, 0,
            new_ir2(
                IR_COMPARES,  size,
                left, right
            ),
            new_ir2(
                IR_PAIR, 0,
                new_bbir(truebb),
                new_bbir(falsebb)
            )
        )
    );
}

static void compare0_branch2(int opcode,
    struct basicblock* truebb, struct basicblock* falsebb,
    int irop)
{
    push(
        new_wordir(0)
    );

    simple_branch2(opcode, EM_wordsize, truebb, falsebb, irop);
}

static void insn_bvalue(int opcode, struct basicblock* leftbb, struct basicblock* rightbb)
{
    switch (opcode)
    {
        case op_zeq: compare0_branch2(opcode, leftbb, rightbb, IR_CJUMPEQ); break;
        case op_zlt: compare0_branch2(opcode, leftbb, rightbb, IR_CJUMPLT); break;
        case op_zle: compare0_branch2(opcode, leftbb, rightbb, IR_CJUMPLE); break;

        case op_zne: compare0_branch2(opcode, rightbb, leftbb, IR_CJUMPEQ); break;
        case op_zge: compare0_branch2(opcode, rightbb, leftbb, IR_CJUMPLT); break;
        case op_zgt: compare0_branch2(opcode, rightbb, leftbb, IR_CJUMPLE); break;

        case op_beq: simple_branch2(opcode, EM_wordsize, leftbb, rightbb, IR_CJUMPEQ); break;
        case op_blt: simple_branch2(opcode, EM_wordsize, leftbb, rightbb, IR_CJUMPLT); break;
        case op_ble: simple_branch2(opcode, EM_wordsize, leftbb, rightbb, IR_CJUMPLE); break;

        case op_bne: simple_branch2(opcode, EM_wordsize, rightbb, leftbb, IR_CJUMPEQ); break;
        case op_bge: simple_branch2(opcode, EM_wordsize, rightbb, leftbb, IR_CJUMPLT); break;
        case op_bgt: simple_branch2(opcode, EM_wordsize, rightbb, leftbb, IR_CJUMPLE); break;

        case op_bra:
        {
            materialise_stack();

            appendir(
                new_ir1(
                    IR_JUMP, 0,
                    new_bbir(leftbb)
                )
            );
            break;
        }

        case op_lae:
            push(
                new_bbir(leftbb)
            );
            break;

        default:
            fatal("treebuilder: unknown bvalue instruction '%s'",
                em_mnem[opcode - sp_fmnem]);
    }
}

static void simple_alu1(int opcode, int size, int irop)
{
    struct ir* val = pop(size);

    push(
        new_ir1(
            irop, size,
            val
        )
    );
}

static void simple_alu2(int opcode, int size, int irop)
{
    struct ir* right = pop(size);
    struct ir* left = pop(size);

    push(
        new_ir2(
            irop, size,
            left, right
        )
    );
}

static struct ir* extract_block_refs(struct basicblock* bb)
{
    struct ir* outir = NULL;
    int i;

    for (i=0; i<bb->ems.count; i++)
    {
        struct em* em = bb->ems.item[i];
        assert(em->opcode == op_bra);
        assert(em->paramtype == PARAM_BVALUE);

        outir = new_ir2(
            IR_PAIR, 0,
            new_bbir(em->u.bvalue.left),
            outir
        );
    }

    return outir;
}

static void change_by(struct ir* address, int amount)
{
    appendir(
        new_ir2(
            IR_STORE, EM_wordsize,
            address,
            new_ir2(
                IR_ADD, EM_wordsize,
                new_ir1(
                    IR_LOAD, EM_wordsize,
                    address
                ),
                new_wordir(amount)
            )
        )
    );
}

static void insn_ivalue(int opcode, arith value)
{
    switch (opcode)
    {
        case op_adi: simple_alu2(opcode, value, IR_ADD); break;
        case op_sbi: simple_alu2(opcode, value, IR_SUB); break;
        case op_mli: simple_alu2(opcode, value, IR_MUL); break;
        case op_dvi: simple_alu2(opcode, value, IR_DIV); break;
        case op_rmi: simple_alu2(opcode, value, IR_MOD); break;
        case op_ngi: simple_alu1(opcode, value, IR_NEG); break;

        case op_and: simple_alu2(opcode, value, IR_AND); break;
        case op_ior: simple_alu2(opcode, value, IR_OR); break;
        case op_xor: simple_alu2(opcode, value, IR_EOR); break;
        case op_com: simple_alu1(opcode, value, IR_NOT); break;

        case op_adf: simple_alu2(opcode, value, IR_ADDF); break;
        case op_sbf: simple_alu2(opcode, value, IR_SUBF); break;
        case op_mlf: simple_alu2(opcode, value, IR_MULF); break;
        case op_dvf: simple_alu2(opcode, value, IR_DIVF); break;
        case op_ngf: simple_alu1(opcode, value, IR_NEGF); break;

        case op_lol:
            push(
                new_ir1(
                    IR_LOAD, EM_wordsize,
                    new_localir(value)
                )
            );
            break;

        case op_stl:
            appendir(
                new_ir2(
                    IR_STORE, EM_wordsize,
                    new_localir(value),
                    pop(EM_wordsize)
                )
            );
            break;

        case op_lal:
            push(
                new_localir(value)
            );
            break;

        case op_inl:
            change_by(new_localir(value), 1);
            break;

        case op_del:
            change_by(new_localir(value), -1);
            break;

        case op_zrl:
            appendir(
                new_ir2(
                    IR_STORE, EM_wordsize,
                    new_localir(value),
                    new_wordir(0)
                )
            );
            break;

        case op_loc:
            push(
                new_wordir(value)
            );
            break;

        case op_loi:
            push(
                new_ir1(
                    IR_LOAD, value,
                    pop(EM_pointersize)
                )
            );
            break;

        case op_sti:
        {
            struct ir* ptr = pop(EM_pointersize);
            struct ir* val = pop(value);

            appendir(
                new_ir2(
                    IR_STORE, value,
                    ptr, val
                )
            );
            break;
        }

        case op_cmi:
            push(
                tristate_compare(value, IR_COMPARES)
            );
            break;

        case op_cmu:
            push(
                tristate_compare(value, IR_COMPAREU)
            );
            break;

        case op_ads:
        {
            struct ir* off = pop(value);
            struct ir* ptr = pop(EM_pointersize);

            if (value != EM_pointersize)
                off = convert(off, EM_pointersize, IR_CII1);

            push(
                new_ir2(
                    IR_ADD, EM_pointersize,
                    ptr, off
                )
            );
            break;
        }

        case op_adp:
        {
            struct ir* ptr = pop(EM_pointersize);

            push(
                new_ir2(
                    IR_ADD, EM_pointersize,
                    ptr,
                    new_wordir(value)
                )
            );
            break;
        }

        case op_sbs:
        {
            struct ir* right = pop(EM_pointersize);
            struct ir* left = pop(EM_pointersize);

            struct ir* delta = 
                new_ir2(
                    IR_SUB, EM_pointersize,
                    left, right
                );

            if (value != EM_pointersize)
                delta = convert(delta, value, IR_CII1);

            push(delta);
            break;
        }
            
        case op_dup:
        {
            struct ir* v = pop(value);
            appendir(v);
            push(v);
            push(v);
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
                        struct ir* ir = pop(stack[stackptr-1]->size);
                        value -= ir->size;
                    }

                    if (value != 0)
                    {
                        appendir(
                            new_ir1(
                                IR_STACKADJUST, EM_pointersize,
                                new_wordir(value)
                            )
                        );
                    }
                    break;
            }
            break;
        }

        case op_ret:
        {
            if (value > 0)
            {
                struct ir* retval = pop(value);
                materialise_stack();
                appendir(
                    new_ir1(
                        IR_SETRET, value,
                        retval
                    )
                );
            }

            appendir(
                new_ir0(
                    IR_RET, 0
                )
            );
            break;
        }
                    
        case op_lfr:
        {
            push(
                appendir(
                    new_ir0(
                        IR_GETRET, value
                    )
                )
            );
            break;
        }

        case op_csa:
        case op_csb:
        {
            const char* helper = aprintf(".%s%d",
                (opcode == op_csa) ? "csa" : "csb",
                value);
            struct ir* descriptor = pop(EM_pointersize);

            if (descriptor->opcode != IR_LABEL)
                fatal("csa/csb are only supported if they refer "
                    "directly to a descriptor block");

            push(descriptor);
            materialise_stack();
            appendir(
                new_ir2(
                    IR_JUMP, 0,
                    new_labelir(helper),
                    extract_block_refs(bb_get(descriptor->u.lvalue))
                )
            );
            break;
        }

        default:
            fatal("treebuilder: unknown ivalue instruction '%s'",
                em_mnem[opcode - sp_fmnem]);
    }
}

static void insn_lvalue(int opcode, const char* label, arith offset)
{
    switch (opcode)
    {
        case op_lae:
            push(
                address_of_external(label, offset)
            );
            break;

        case op_loe:
            push(
                new_ir1(
                    IR_LOAD, EM_wordsize,
                    address_of_external(label, offset)
                )
            );
            break;

        case op_ste:
            appendir(
                new_ir2(
                    IR_STORE, EM_wordsize,
                    address_of_external(label, offset),
                    pop(EM_wordsize)
                )
            );
            break;

        case op_cal:
            assert(offset == 0);
            materialise_stack();
            appendir(
                new_ir1(
                    IR_CALL, 0,
                    new_labelir(label)
                )
            );
            break;

        case op_bra:
            assert(offset == 0);
            materialise_stack();
            appendir(
                new_ir1(
                    IR_JUMP, 0,
                    new_labelir(label)
                )
            );
            break;
                    
        default:
            fatal("treebuilder: unknown lvalue instruction '%s'",
                em_mnem[opcode - sp_fmnem]);
    }
}

static void generate_tree(struct basicblock* bb)
{
    int i;

    tracef('0', "0: block %s\n", bb->name);

    current_bb = bb;
    reset_stack();

    for (i=0; i<bb->ems.count; i++)
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
                tracef('E', "label=%s offset=%d\n", 
                    em->u.lvalue.label, em->u.lvalue.offset);
                insn_lvalue(em->opcode, em->u.lvalue.label, em->u.lvalue.offset);
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

    assert(stackptr == 0);
}

void tb_procedure(struct procedure* current_proc)
{
    int i;

    for (i=0; i<current_proc->blocks.count; i++)
        generate_tree(current_proc->blocks.item[i]);

}

/* vim: set sw=4 ts=4 expandtab : */

