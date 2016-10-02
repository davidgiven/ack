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
    struct basicblock* root_bb;
    size_t nlocals;
    ARRAYOF(struct basicblock) blocks;
    IMAPOF(struct local) locals;
};

extern void procedure_compile(struct procedure* proc);
extern void procedure_update_bb_graph(struct procedure* proc);

#endif

/* vim: set sw=4 ts=4 expandtab : */
