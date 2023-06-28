#ifndef REG_H
#define REG_H

#define WITH_ATTR(a) (1<<(a))

struct hreg
{
	const char* id;
    const struct burm_register_data* brd;
	uint32_t attrs;
    uint32_t usage;
};

struct congruence
{
    int id;
    ARRAYOF(struct vreg) vregs;
    uint32_t type;
    int offset;
};

struct vreg
{
	int id;
    uint32_t type;
    struct congruence* congruence;
    struct hop* defined;
    ARRAYOF(struct hop) used;
};

extern void clear_registers(void);
extern int get_vreg_count(void);

extern struct vreg* new_vreg(void);

extern struct hreg* new_hreg(const struct burm_register_data* brd);

extern const char* render_regattrs(uint32_t attrs);

#endif

/* vim: set sw=4 ts=4 expandtab : */
