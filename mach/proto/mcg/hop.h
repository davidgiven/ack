#ifndef HOP_H
#define HOP_H

enum insel_type
{
	INSEL_STRING,
	INSEL_HREG,
	INSEL_VREG,
	INSEL_VALUE,
	INSEL_ST_OFFSET,
	INSEL_AB_OFFSET,
	INSEL_LB_OFFSET,
	INSEL_EOI
};

struct insel
{
	enum insel_type type;
	int index;
	union
	{
		const char* string;
		struct hreg* hreg;
		struct vreg* vreg;
		struct ir* value;
		int offset;
	} u;
};

struct constraint
{
	/* Register must have all of these attributes. */

	uint32_t attrs;

	/* For input registers: this instruction may not modify this register during
	 * execution. */

	bool preserved;

	/* For output registers: this register must be the same as the specified
	 * input register. */

	struct vreg* equals_to;
};

struct move
{
	struct vreg* vreg;
	struct hreg* hreg;
    struct hreg* other;
};

struct hop
{
	int id;
	struct basicblock* bb;
	struct ir* ir;
	const struct burm_instruction_data* insndata;
	ARRAYOF(struct insel) insels;
	struct vreg* output;

	PMAPOF(struct vreg, struct constraint) constraints;

	ARRAYOF(struct vreg) ins;
	ARRAYOF(struct vreg) outs;
	ARRAYOF(struct vreg) throughs;

	ARRAYOF(struct move) produces;
	ARRAYOF(struct move) consumes;
    PMAPOF(struct vreg, struct hreg) assignments;
	uint32_t inputregusage;
	uint32_t outputregusage;
};

extern struct hop* new_hop(struct basicblock* bb, struct ir* ir);

extern void hop_add_string_insel(struct hop* hop, const char* string);
extern void hop_add_hreg_insel(struct hop* hop, struct hreg* hreg, int index);
extern void hop_add_vreg_insel(struct hop* hop, struct vreg* vreg, int index);
extern void hop_add_value_insel(struct hop* hop, struct ir* ir);
extern void hop_add_st_offset_insel(struct hop* hop, struct hreg* hreg);
extern void hop_add_ab_offset_insel(struct hop* hop, int offset);
extern void hop_add_lb_offset_insel(struct hop* hop, int offset);
extern void hop_add_eoi_insel(struct hop* hop);

extern void hop_add_insel(struct hop* hop, const char* fmt, ...);

extern char* hop_render(struct hop* hop);
extern void hop_print(char k, struct hop* hop);

#endif

/* vim: set sw=4 ts=4 expandtab : */
