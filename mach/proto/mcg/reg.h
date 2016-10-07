#ifndef REG_H
#define REG_H

#define WITH_ATTR(a) (1<<(a))

struct hreg
{
	const char* name;
	uint32_t attrs;
	bool is_stacked;
	int offset;
};

struct vreg
{
	int id;
    struct hop* defined;
    ARRAYOF(struct hop) used;
};

extern struct vreg* new_vreg(void);

extern struct hreg* new_hreg(struct burm_register_data* brd);
extern struct hreg* new_stacked_hreg(int offset, uint32_t attrs);

#endif

/* vim: set sw=4 ts=4 expandtab : */
