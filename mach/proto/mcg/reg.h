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
};

extern struct hreg* new_hreg(const struct burm_register_data* brd);
extern struct hreg* new_stacked_hreg(uint32_t type);

#endif

/* vim: set sw=4 ts=4 expandtab : */
