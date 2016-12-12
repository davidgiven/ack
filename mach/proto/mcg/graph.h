#ifndef GRAPH_H
#define GRAPH_H

struct graph_data
{
    struct basicblock* entry;
    PMAPOF(struct basicblock, struct basicblock) graph;
    ARRAYOF(struct basicblock) preorder;
    ARRAYOF(struct basicblock) postorder;
};

struct dominance_data
{
    PMAPOF(struct basicblock, struct basicblock) graph;
    ARRAYOF(struct basicblock) preorder;
    ARRAYOF(struct basicblock) postorder;
    PMAPOF(struct basicblock, struct basicblock) frontiers;
};

extern struct graph_data cfg;
extern struct dominance_data dominance;

extern void update_graph_data(void);

#endif

/* vim: set sw=4 ts=4 expandtab : */

