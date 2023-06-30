#ifndef REG_H
#define REG_H

#define WITH_ATTR(a) (1<<(a))

struct hreg
{
	const char* id;
    const struct burm_register_data* brd;
	regclasses_t regclasses;
    uint32_t usage;
};

struct congruence
{
    int id;
    ARRAYOF(struct vreg) vregs;
    regclasses_t regclasses;
    int offset;
};

struct vreg
{
	int id;
    regclass_t regclass;
    struct congruence* congruence;
    struct hop* defined;
    ARRAYOF(struct hop) used;
    bool in_transit;
    struct vreg* coalesced_with;
    struct vreg* next_coalesced_register;
};

extern void clear_registers(void);
extern int vreg_count;

extern struct vreg* new_vreg(void);

extern struct hreg* new_hreg(const struct burm_register_data* brd);

extern const char* render_regclass(regclass_t regclass);
extern const char* render_regclasses(regclasses_t regclasses);

#endif

/* vim: set sw=4 ts=4 expandtab : */
