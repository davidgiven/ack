#ifndef HOP_H
#define HOP_H

typedef uint64_t regmask_t;

enum insel_type
{
	INSEL_STRING, /* %s */
	INSEL_HREG, /* %H */
	INSEL_VREG, /* %V */
	INSEL_VALUE,
	INSEL_SB_OFFSET, /* %S */
	INSEL_AB_OFFSET, /* %A */
	INSEL_LB_OFFSET, /* %L */
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
	/* The required register class for input parameters. */

	regclass_t regclass;

	/* For input registers: this instruction may not modify this register during
	 * execution. */

	bool preserved;

	/* For input or output registers: this register must be the same as the
	 * specified output or input register. */

	struct vreg* equals_to;
};

struct hop
{
	int id;
	struct basicblock* bb;
	struct ir* ir;
	bool top;
	const struct burm_instruction_data* insndata;
	ARRAYOF(struct insel) insels;
	struct vreg* output;

	PMAPOF(struct vreg, struct constraint) constraints;

	ARRAYOF(struct vreg) ins;
	ARRAYOF(struct vreg) outs;
	ARRAYOF(struct vreg) throughs;

	bool is_copy;
};

extern void clear_hops(void);
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

extern struct constraint* get_constraint(struct hop* hop, struct vreg* vreg);
extern void replace_vregs_in_hop(
    struct hop* hop, struct vreg* oldvreg, struct vreg* newvreg);

extern char* hop_render(struct hop* hop);
extern void hop_print(char k, struct hop* hop);

#endif

/* vim: set sw=4 ts=4 expandtab : */
