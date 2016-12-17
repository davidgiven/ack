#ifndef BIGRAPH_H
#define BIGRAPH_H

#include "hashtable.h"
#include "set.h"

/* A bidirectional graph with node addition and removal capabilities. */

struct graph
{
    struct hashtable vertices;
    struct set edges;
};

struct vertex_iterator
{
    /* Public */
    void* data;

    /* Private */
    struct hashtable_iterator hit;
};

struct edge_iterator
{
    /* Public */
    void* left;
    void* right;

    /* Private */
    struct set_iterator sit;
};

extern void graph_reset(struct graph* g);

extern bool graph_contains_vertex(struct graph* g, void* data);
extern void graph_add_vertex(struct graph* g, void* data);
extern void graph_remove_vertex(struct graph* g, void* data);
extern bool graph_contains_edge(struct graph* g, void* data1, void* data2);
extern void graph_add_edge(struct graph* g, void* data1, void* data2);
extern void graph_remove_edge(struct graph* g, void* data1, void* data2);
extern int graph_get_vertex_degree(struct graph* g, void* data);

extern void* graph_next_vertex(struct graph* g, struct vertex_iterator* it);
extern bool graph_next_edge(struct graph* g, struct edge_iterator* it);

#endif
