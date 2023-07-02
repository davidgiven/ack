#ifndef REG_H
#define REG_H

#define WITH_ATTR(a) (1 << (a))

struct hreg
{
	const char* id;
	const struct burm_register_data* brd;
	regclasses_t regclasses;
	regbits_t usage;
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

	/* Used by register allocation. */

	struct vreg* coalesced_with;
	struct vreg* next_coalesced_register;
	int min_hop;
	int max_hop;
	int use_count;
    int interference_count;
	bool is_spilt;
    struct hreg* hreg;
};

extern void clear_registers(void);
extern int vreg_count;

extern struct vreg* new_vreg(void);

extern struct hreg* new_hreg(const struct burm_register_data* brd);

extern const char* render_regclass(regclass_t regclass);
extern const char* render_regclasses(regclasses_t regclasses);
extern const char* render_vreg(struct vreg* vreg);

#endif

/* vim: set sw=4 ts=4 expandtab : */
