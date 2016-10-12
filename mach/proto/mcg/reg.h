#ifndef REG_H
#define REG_H

#define WITH_ATTR(a) (1<<(a))

struct phicongruence
{
    int id;
    ARRAYOF(struct vreg) vregs;
    ARRAYOF(struct hop) definitions;
    uint32_t attrs;
};

struct hreg
{
	const char* name;
    uint32_t type;
	uint32_t attrs;
	bool is_stacked;
	int offset;
};

struct vreg
{
	int id;
    struct phicongruence* congruence;
    struct hop* defined;
    ARRAYOF(struct hop) used;
};

typedef PMAPOF(struct hreg, struct vreg) register_assignment_t;

extern struct vreg* new_vreg(void);

extern struct hreg* new_hreg(const struct burm_register_data* brd);
extern struct hreg* new_stacked_hreg(int offset, uint32_t attrs);

#endif

/* vim: set sw=4 ts=4 expandtab : */
