#ifndef BASICBLOCK_H
#define BASICBLOCK_H

struct phi
{
    struct basicblock* prev; /* Predecessor that this phi is referring to */
    struct ir* ir;           /* IR of variable definition */
};

struct basicblock
{
    const char* name;
    ARRAYOF(struct em) ems;
    ARRAYOF(struct ir) irs;
    ARRAYOF(struct hop) hops;

    ARRAYOF(struct basicblock) prevs;
    ARRAYOF(struct basicblock) nexts;
    int order; /* used by dominance graph code */

    PMAPOF(struct vreg, struct phi) phis;

    /* Used by liveness calculation. */
    ARRAYOF(struct vreg) liveins;
    ARRAYOF(struct vreg) liveouts;

    /* Register assignments on entry and exit. These are *pointers* (because
     * they just point to the regsin/regsout of the first and last hop
     * respectively). */
    register_assignment_t* regsin;
    register_assignment_t* regsout;

    bool is_fake : 1;
    bool is_root : 1;
    bool is_terminated : 1;
};

extern void bb_init(void);
extern struct basicblock* bb_get(const char* name);
extern void bb_alias(struct basicblock* block, const char* name);

#endif

/* vim: set sw=4 ts=4 expandtab : */

