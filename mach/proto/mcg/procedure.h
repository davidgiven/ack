#ifndef PROCEDURE_H
#define PROCEDURE_H

struct local
{
    int size;
    int offset;
    bool is_register;
};

struct procedure
{
    const char* name;
    struct basicblock* entry;
    struct basicblock* exit;
    int locals_size;
    int spills_size;
    int saved_size;
    int fp_to_ab; /* argument base (indexes up) */
    int fp_to_lb; /* locals base (indexes down) */
    int fp_to_sb; /* spill base (indexes up) */
    int fp_to_rb; /* saved registers base (indexes up) */
    ARRAYOF(struct basicblock) blocks;
    IMAPOF(struct local) locals;
    ARRAYOF(struct hreg) usedregs;
};

extern void procedure_compile(struct procedure* proc);
extern void procedure_update_bb_graph(struct procedure* proc);

extern struct procedure* current_proc;

#endif

/* vim: set sw=4 ts=4 expandtab : */
