#ifndef HOP_H
#define HOP_H

struct vreg
{
	int id;
	int regclass;
};

#define HOP_INOUT_REGS 4

struct hop
{
	int insn_no;
	struct ir* ir;
	struct vreg* invregs[HOP_INOUT_REGS];
	struct vreg* outvregs[HOP_INOUT_REGS];
};

extern struct hop* new_hop(int insn_no, struct ir* ir);

extern void hop_add_in_reg(struct hop* hop, struct vreg* reg);
extern void hop_add_out_reg(struct hop* hop, struct vreg* reg);

#endif


