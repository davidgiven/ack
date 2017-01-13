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
		struct value* value;
		struct ir* ir;
        int offset;
	}
	u;
};

struct hop
{
	int id;
    struct basicblock* bb;
	struct ir* ir;
    const struct burm_instruction_data* insndata;
	ARRAYOF(struct insel) insels;
	struct value* value;
	bool is_move;
	const char* pseudo;
	PMAPOF(struct value, struct value) equals_constraint;

	ARRAYOF(struct value) inputs;
	ARRAYOF(struct value) outputs;
	ARRAYOF(struct value) throughs;
	ARRAYOF(struct value) corrupted;

	struct hashtable* vregmapping;
};

extern struct hop* new_hop(struct basicblock* bb, struct ir* ir);
extern struct hop* new_move_hop(struct basicblock* bb);

extern void hop_add_string_insel(struct hop* hop, const char* string);
extern void hop_add_hreg_insel(struct hop* hop, struct hreg* hreg, int index);
extern void hop_add_vreg_insel(struct hop* hop, struct value* value, int index);
extern void hop_add_value_insel(struct hop* hop, struct ir* ir);
extern void hop_add_st_offset_insel(struct hop* hop, struct hreg* hreg);
extern void hop_add_ab_offset_insel(struct hop* hop, int offset);
extern void hop_add_lb_offset_insel(struct hop* hop, int offset);
extern void hop_add_eoi_insel(struct hop* hop);

extern void hop_add_insel(struct hop* hop, const char* fmt, ...);

typedef void hop_walker_t(struct hop* hop, void* user);
extern void hop_walk(hop_walker_t* callback, void* user);

extern char* hop_render(struct hop* hop);
extern void hop_print(char k, struct hop* hop);

#endif

/* vim: set sw=4 ts=4 expandtab : */

