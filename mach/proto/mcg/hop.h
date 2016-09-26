#ifndef HOP_H
#define HOP_H

struct hop
{
	int insn_no;
	struct ir* ir;
	struct vreg* resultvreg;
	ARRAYOF(struct vreg) invregs;
	ARRAYOF(struct vreg) outvregs;
	ARRAYOF(struct vreg) newvregs;
	ARRAYOF(struct vreg) deadvregs;
};

extern struct hop* new_hop(int insn_no, struct ir* ir);

extern void hop_print(char k, struct hop* hop);

#endif


