#ifndef HOP_H
#define HOP_H

enum insel_type
{
	INSEL_STRING,
	INSEL_VREG,
	INSEL_VALUE,
	INSEL_EOI
};

struct insel
{
	enum insel_type type;
	union
	{
		const char* string;
		struct vreg* vreg;
		struct ir* value;
	}
	u;
};

struct hop
{
	int id;
	int insn_no;
	struct ir* ir;
	ARRAYOF(struct insel) insels;
	struct vreg* output;
	PMAPOF(struct vreg, struct hreg) registers;
};

extern struct hop* new_hop(int insn_no, struct ir* ir);

extern void hop_add_string_insel(struct hop* hop, const char* string);
extern void hop_add_vreg_insel(struct hop* hop, struct vreg* vreg);
extern void hop_add_value_insel(struct hop* hop, struct ir* ir);
extern void hop_add_eoi_insel(struct hop* hop);

extern void hop_print(char k, struct hop* hop);

#endif

/* vim: set sw=4 ts=4 expandtab : */

