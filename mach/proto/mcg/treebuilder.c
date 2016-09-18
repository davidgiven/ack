#include "mcg.h"

static struct symbol* currentproc;
static struct basicblock* rootbb;
static struct basicblock* currentbb;

static int stackptr;
static struct ir* stack[64];

static void resetstack(void)
{
    stackptr = 0;
}

static void push(struct ir* ir)
{
    if (stackptr == sizeof(stack)/sizeof(*stack))
        fatal("stack overflow");

    stack[stackptr++] = ir;
}

static struct ir* pop(void)
{
    if (stackptr == 0)
        fatal("stack underflow");

    return stack[--stackptr];
}

static struct ir* appendir(struct ir* ir)
{
    assert(currentbb != NULL);
    ir->sequence = true;
    APPEND(currentbb->irs, ir);

    ir_print(ir);
    return ir;
}

void tb_filestart(void)
{
}

void tb_fileend(void)
{
}

static void materialise(void)
{
    int i;

    for (i=0; i<stackptr; i++)
        appendir(stack[i]);
}

static void changeblock(struct basicblock* bb)
{
    int i;

    if (stackptr > 0)
    {
        printf("\t; block exiting with %d on stack:\n", stackptr);
        for (i=0; i<stackptr; i++)
        {
            struct ir* ir = stack[i];
            printf("\t;   $%d size %d\n", ir->id, ir->size);
            APPENDU(currentbb->outs, ir);
        }
    }

    for (i=0; i<currentbb->outblocks_count; i++)
        bb_wire_outs_to_ins(currentbb, currentbb->outblocks[i]);

    currentbb = bb;
    printf("; new block: %s\n", currentbb->name);

    resetstack();

    if (currentbb->ins_count > 0)
    {
        printf("\t; block entering with %d on stack:\n", currentbb->ins_count);
        for (i=0; i<currentbb->ins_count; i++)
        {
            struct ir* ir = currentbb->ins[i];
            printf("\t;   $%d size %d\n", ir->id, ir->size);
            push(ir);
        }
    }
}

void tb_ilabel(const char* label)
{
    materialise();

    #if 0
    if (currentbb->irs_count == 0)
    {
        /* Current BB has no instructions, so just alias it to the
         * new name.
         */
        bb_alias(currentbb, label);
    }
    else
    #endif
    {
        struct basicblock* newbb = bb_get(label);

        if ((currentbb->irs_count == 0) ||
            !currentbb->irs[currentbb->irs_count-1]->terminates)
        {
            APPEND(currentbb->outblocks, newbb);
            appendir(
                new_ir1(
                    IR_JUMP, 0,
                    new_labelir(label)
                )
            );
        }

        changeblock(newbb);
    }
}

void tb_procstart(const char* label, size_t nlocals)
{
    assert(currentproc == NULL);

    currentproc = symbol_get(label);
    currentproc->section = SECTION_TEXT;

    rootbb = calloc(sizeof(struct basicblock), 1);
    currentbb = rootbb;

    resetstack();
}

void tb_procend(void)
{
    assert(currentproc != NULL);

    printf("\n.text\n");
    printf("%s:\n", currentproc->name);

    currentproc = NULL;
}

void tb_regvar(arith offset, int size, int type, int priority)
{
    /* ignored */
}

static struct ir* address_of_local(int index)
{
    return
        new_ir2(
            IR_ADD, EM_pointersize,
            new_regir((index < 0) ? IRR_LB : IRR_AB),
            new_wordir(index)
        );
}

static struct ir* tristate_compare(int size, int opcode)
{
    struct ir* right = pop();
    struct ir* left = pop();

    return
        new_ir2(
            opcode, size,
            left, right
        );
}

static struct ir* convert(int destsize, int srcsize, int opcode)
{
    switch (srcsize)
    {
        case 1: opcode += 0; break;
        case 2: opcode += 1; break;
        case 4: opcode += 2; break;
        case 8: opcode += 3; break;
        default:
            fatal("can't convert from things of size %d", srcsize);
    }

    return
        new_ir1(
            opcode, destsize,
            pop()
        );
}

void tb_insn_simple(int opcode, int flags)
{
    switch (opcode)
    {
        case op_cii:
        {
            struct ir* destsize = pop();
            struct ir* srcsize = pop();

            assert(srcsize->opcode == IR_ICONST);
            assert(destsize->opcode == IR_ICONST);

            push(
                convert(destsize->u.ivalue, srcsize->u.ivalue, IR_FROMI1)
            );
            break;
        }

        default:
            fatal("unknown insn_simple instruction '%s'",
                em_mnem[opcode - sp_fmnem]);
    }
}

void tb_insn_label(int opcode, int flags, const char* label, arith offset)
{
    materialise();

    switch (opcode)
    {
        case op_zne:
        {
            struct basicblock* truebb = bb_get(label);
            struct basicblock* falsebb = bb_get(NULL);

            APPENDU(currentbb->outblocks, truebb);
            APPENDU(currentbb->outblocks, falsebb);

            appendir(
                new_ir3(
                    IR_CJUMP, 0,
                    pop(),
                    new_bbir(truebb),
                    new_bbir(falsebb)
                )
            );

            changeblock(falsebb);
            break;
        }

        case op_bra:
        {
            struct basicblock* destbb = bb_get(label);
            APPENDU(currentbb->outblocks, destbb);

            appendir(
                new_ir1(
                    IR_JUMP, 0,
                    new_bbir(destbb)
                )
            );
            break;
        }

        default:
            fatal("unknown insn_label instruction '%s'",
                em_mnem[opcode - sp_fmnem]);
    }
}

void tb_insn_value(int opcode, int flags, arith value)
{
    struct ir* left;
    struct ir* right;

    switch (opcode)
    {
        case op_lol:
            push(
                new_ir1(
                    IR_LOAD, EM_wordsize,
                    address_of_local(value)
                )
            );
            break;

        case op_stl:
            appendir(
                new_ir2(
                    IR_STORE, EM_wordsize,
                    address_of_local(value),
                    pop()
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
                    pop()
                )
            );
            break;

        case op_sti:
            right = pop();
            left = pop();

            appendir(
                new_ir2(
                    IR_STORE, value,
                    right, left
                )
            );
            break;

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
            right = pop();
            left = pop();

            if (value != EM_pointersize)
                right = convert(EM_pointersize, value, IR_FROMI1);

            push(
                new_ir2(
                    IR_ADD, EM_wordsize,
                    left, right
                )
            );
            break;

        case op_dup:
        {
            struct ir* v = pop();
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
                    appendir(
                        new_ir2(
                            IR_SETREG, EM_pointersize,
                            new_regir(IRR_SP),
                            new_ir2(
                                IR_ADD, EM_pointersize,
                                new_regir(IRR_SP),
                                new_wordir(value)
                            )
                        )
                    );
                    break;
            }
            break;
        }

        case op_ret:
        {
            if (value > 0)
            {
                left = pop();
                assert(left->size == value);
                appendir(
                    new_ir2(
                        IR_SETREG, value,
                        new_regir(IRR_RR),
                        left
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
                    
        default:
            fatal("unknown insn_value instruction '%s'",
                em_mnem[opcode - sp_fmnem]);
    }
}

/* vim: set sw=4 ts=4 expandtab : */

