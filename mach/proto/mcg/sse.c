#include "mcg.h"

static struct ir* get_last_push(struct basicblock* bb)
{
    int i;

    for (i=bb->irs_count-1; i>=0; i--)
    {
        struct ir* ir = bb->irs[i];

        if (ir->opcode == IR_PUSH)
            return ir;
        if (ir_find(ir, IR_POP))
            return NULL;
    }

    return NULL;
}

static struct ir* get_first_pop(struct basicblock* bb)
{
    int i;

    for (i=0; i<bb->irs_count; i++)
    {
        struct ir* irr;
        struct ir* ir = bb->irs[i];

        if (ir->opcode == IR_PUSH)
            return NULL;

        irr = ir_find(ir, IR_POP);
        if (irr)
            return irr;
    }

    return NULL;
}

void sse_convert_block_parameters(struct basicblock* bb)
{
    int i, j;
    struct ir* ir;

    for (;;)
    {
        struct ir* lastpush = get_last_push(bb);
        if (!lastpush)
            return;

        /* Abort unless *every* success block of this one starts with a pop
         * of the same size... */

        for (i=0; i<bb->outblocks_count; i++)
        {
            struct basicblock* outbb = bb->outblocks[i];

            ir = get_first_pop(outbb);
            if (!ir || (ir->size != lastpush->size))
                return;

            /* Also abort unless *every* predecessor block of the one we've
             * just found *also* ends in a push of the same size. */

            for (j=0; j<outbb->inblocks_count; j++)
            {
                struct basicblock* inbb = outbb->inblocks[i];

                ir = get_last_push(inbb);
                if (!ir || (ir->size != lastpush->size))
                    return;
            }
        }

        /* Okay, now we can wire them all up. */

        for (i=0; i<bb->outblocks_count; i++)
        {
            struct basicblock* outbb = bb->outblocks[i];
            struct ir* phi = get_first_pop(outbb);
            phi->opcode = IR_PHI;

            /* Also abort unless *every* predecessor block of the one we've
             * just found *also* ends in a push of the same size. */

            for (j=0; j<outbb->inblocks_count; j++)
            {
                struct basicblock* inbb = outbb->inblocks[j];

                ir = get_last_push(inbb);
                *ir = *ir->left;
                ir->is_sequence = true;
                APPEND(phi->u.phivalue.imports, ir);
            }
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */
