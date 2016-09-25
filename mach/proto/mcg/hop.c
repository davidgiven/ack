#include "mcg.h"

struct hop* new_hop(int insn_no, struct ir* ir)
{
	struct hop* hop = calloc(1, sizeof *hop);
	hop->insn_no = insn_no;
	hop->ir = ir;
}

static void add_reg(struct vreg* vregs[], struct vreg* vreg)
{
	int i;

	for (i=0; i<HOP_INOUT_REGS; i++)
		if (!vregs[i])
		{
			vregs[i] = vreg;
			return;
		}

	fatal("too many input registers for instruction");
}

void hop_add_in_reg(struct hop* hop, struct vreg* vreg)
{
	add_reg(hop->invregs, vreg);
}

void hop_add_out_reg(struct hop* hop, struct vreg* vreg)
{
	add_reg(hop->outvregs, vreg);
}

void hop_emit(struct hop* hop, const struct burm_emitter_data* ed)
{
    const struct burm_instruction_data* insndata = &burm_instruction_data[hop->insn_no];
	insndata->emitter(hop->ir, ed);
}

