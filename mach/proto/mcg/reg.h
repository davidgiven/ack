#ifndef REG_H
#define REG_H

#define WITH_ATTR(a) (1<<(a))

struct hreg
{
	const char* id;
    const struct burm_register_data* brd;
	uint32_t attrs;
	bool is_stacked;
	int offset;
    ARRAYOF(struct hreg) aliases;
};

struct vreg
{
    int id;
    struct value* value;
    struct vreg* coalesced_with;
    int hreg;
    int neighbours;
    bool needs_register;
    bool is_spilt;
    const struct burm_regclass_data* regclass;
    burm_register_bitmap_t registers;
};

extern struct hreg* new_hreg(const struct burm_register_data* brd);
extern struct hreg* new_stacked_hreg(uint32_t type);

#endif

/* vim: set sw=4 ts=4 expandtab : */
