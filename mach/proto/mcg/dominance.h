#ifndef DOMINANCE_H
#define DOMINANCE_H

extern struct array postorder;
extern struct array preorder;
extern struct pmap dominators;

extern void calculate_dominance_graph(struct procedure* proc);

#endif

/* vim: set sw=4 ts=4 expandtab : */

