#ifndef PROCEDURE_H
#define PROCEDURE_H

struct local
{
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

#endif

/* vim: set sw=4 ts=4 expandtab : */
