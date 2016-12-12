#include "mcg.h"

/* The ACK returns values from functions not on the stack but in a special
 * 'register' which are read with lfr.  This register is defined to survive
 * asp, bra and gto. The way it's intended to work is that value just gets put
 * in a particular hreg and stays there until lfr brings it to the attention of
 * the code generator.
 *
 * Trouble is, while that worked on ncg, it doesn't work here because the
 * register allocator may decide to insert moves arbitrarily. So we need to
 * somehow turn this special register into a real register so that it can be
 * kept alive.
 *
 * The easiest thing to do is to just push the result of call onto the stack...
 * but that doesn't work either, because if someone does a call without an lfr,
 * we don't want to be left with an unpopped value.
 *
 * So what we do is we find lfrs, and then we search for the call which
 * generated the value, and then we hook up the IRs so there's a connection
 * between the two. But beware! The lfr value survives bra! Which means a
 * single lfr may actually read the value produced by *several* call
 * instructions. You know what that means? Phis.
 *
 * (Luckily a single call instruction can't be read by multiple lfrs, because
 * conditional branches trash the lfr value.)
 */

static void find_call(struct basicblock* bb, int index, struct ir* lfr,
        struct basicblock** callbb, struct ir** callir)
{
    if (index == -1)
        index = bb->irs.count - 1;

    while (index >= 0)
    {
        struct ir* ir = bb->irs.item[index];
        switch (ir->opcode)
        {
            case IR_CALL:
                ir->size = lfr->size;
                *callbb = bb;
                *callir = ir;
                return;

            case IR_STACKADJUST:
            case IR_GETRET:
            case IR_JUMP:
                /* lfr value preserved */
                break;

            default:
                /* lfr value has been corrupted. */
                fatal("lfr reading corrupted value in %s", bb->name);
        }

        index--;
    }

    /* Our search hit the top of the block; we need to import the
     * lfr value from a previous block. */

    if (bb->prevs.count == 1)
    {
        /* Only a single predecessor, so no phi is necessary. */

        find_call(bb->prevs.item[0], -1, lfr, callbb, callir);
    }
    else
    {
        /* We have multiple predecessors. This means that the lfr value may
         * come from any of these blocks. We need a phi. */

        int i;
        struct ir* phi = new_ir0(IR_PHI, lfr->size);

        phi->root = phi;
        phi->bb = bb;
        array_insert(&bb->irs, phi, 0);

        for (i=0; i<bb->prevs.count; i++)
        {
            struct basicblock* prev = bb->prevs.item[i];
            struct basicblock* parentbb;
            struct ir* parentir;

            find_call(prev, -1, phi, &parentbb, &parentir);

            pmap_add(&phi->u.phivalue, parentbb, parentir);
        }

        *callbb = bb;
        *callir = phi;
    }
}

static void wire_up_ir(struct basicblock* bb, int index)
{
    struct ir* lfr = bb->irs.item[index];
    struct basicblock* callbb;
    struct ir* callir;

    find_call(bb, index, lfr, &callbb, &callir);

    lfr->left = callir;
    lfr->opcode = IR_NOP;
}

void pass_wire_up_return_values(void)
{
    int i, j;

    for (i=0; i<cfg.preorder.count; i++)
    {
        struct basicblock* bb = cfg.preorder.item[i];
        for (j=0; j<bb->irs.count; j++)
        {
            struct ir* ir = bb->irs.item[j];
            if (ir->opcode == IR_GETRET)
                wire_up_ir(bb, j);
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */
