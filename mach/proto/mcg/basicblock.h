#ifndef BASICBLOCK_H
#define BASICBLOCK_H

struct basicblock
{
    const char* name;
    ARRAYOF(struct em) ems;
    ARRAYOF(struct ir) irs;
    ARRAYOF(struct hop) hops;

    ARRAYOF(struct basicblock) prevs;
    ARRAYOF(struct basicblock) nexts;
    int order; /* used by SSA code */

    bool is_fake : 1;
    bool is_root : 1;
    bool is_terminated : 1;
};

extern void bb_init(void);
extern struct basicblock* bb_get(const char* name);
extern void bb_alias(struct basicblock* block, const char* name);

#endif

