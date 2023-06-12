#ifndef REG_H
#define REG_H

#define WITH_ATTR(a) (1<<(a))

struct congruence
{
    int id;
    ARRAYOF(struct vreg) vregs;
    uint32_t type;
    struct hreg* evicted; /* stack slot to evict to */
};

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
    uint32_t type;
    struct congruence* congruence;
    struct hop* defined;
    ARRAYOF(struct hop) used;
    struct hreg* evicted; /* stack slot to evict to */
};

typedef PMAPOF(struct hreg, struct vreg) register_assignment_t;

extern struct vreg* new_vreg(void);

extern struct hreg* new_hreg(const struct burm_register_data* brd);
extern struct hreg* new_stacked_hreg(uint32_t type);

#endif

/* vim: set sw=4 ts=4 expandtab : */
