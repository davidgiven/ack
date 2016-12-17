#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "bigraph.h"
#include "set.h"

struct edge
{
    struct vertex* v1;
    struct vertex* v2;
};

struct vertex
{
    void* data;
    int degree;
	struct set edges;
};

static uint32_t edge_hash_function(void* key)
{
    struct edge* edge = key;
    /* This will always return the same value, even if the two endpoints are swapped. */
    return standard_pointer_hash_function(edge->v1) ^ standard_pointer_hash_function(edge->v2);
}

static bool edge_comparison_function(void* key1, void* key2)
{
    struct edge* edge1 = key1;
    struct edge* edge2 = key2;

    return ((edge1->v1 == edge2->v1) && (edge1->v2 == edge2->v2))
        || ((edge1->v1 == edge2->v2) && (edge1->v2 == edge2->v1));
}

static void lazy_init(struct graph* g)
{
    g->edges.table.hashfunction = edge_hash_function;
    g->edges.table.cmpfunction = edge_comparison_function;
}

void graph_reset(struct graph* g)
{
    lazy_init(g);

    for (;;)
    {
        struct vertex* vertex = hashtable_pop(&g->vertices);
        if (!vertex)
            return;

		set_reset(&vertex->edges);
        free(vertex);
    }

	for (;;)
	{
		struct edge* edge = set_pop(&g->edges);
		free(edge);
	}
}

bool graph_contains_vertex(struct graph* g, void* data)
{
    lazy_init(g);

    return !!hashtable_get(&g->vertices, data);
}

static struct vertex* find_or_add_vertex(struct graph* g, void* data)
{
    struct vertex* vertex;

    lazy_init(g);

    vertex = hashtable_get(&g->vertices, data);
    if (!vertex)
    {
        vertex = calloc(1, sizeof(struct vertex));
        vertex->data = data;
        hashtable_put(&g->vertices, data, vertex);
    }
    
    return vertex;
}

bool graph_contains_edge(struct graph* g, void* data1, void* data2)
{
	struct edge template;


    template.v1 = find_or_add_vertex(g, data1);
    template.v2 = find_or_add_vertex(g, data2);
	if (!template.v1 || !template.v2)
		return false;

	return !!set_get(&g->edges, &template);
}

void graph_add_edge(struct graph* g, void* data1, void* data2)
{
	struct edge template;
	struct edge* e;

    template.v1 = find_or_add_vertex(g, data1);
    template.v2 = find_or_add_vertex(g, data2);
	if (template.v1 == template.v2)
		return;

	if (set_get(&g->edges, &template))
		return;

	e = calloc(1, sizeof(struct edge));
	*e = template;

	set_add(&g->edges, e);
	set_add(&template.v1->edges, e);
	set_add(&template.v2->edges, e);
	template.v1->degree++;
	template.v2->degree++;
}

void graph_remove_edge(struct graph* g, void* data1, void* data2)
{
	struct edge template;
    struct vertex* v1 = find_or_add_vertex(g, data1);
    struct vertex* v2 = find_or_add_vertex(g, data2);

    template.v1 = find_or_add_vertex(g, data1);
    template.v2 = find_or_add_vertex(g, data2);
	if (template.v1 == template.v2)
		return;

	set_remove(&template.v1->edges, &template);
	set_remove(&template.v2->edges, &template);
	free(set_remove(&g->edges, &template));
	template.v1->degree--;
	template.v2->degree--;
}

void graph_add_vertex(struct graph* g, void* data)
{
    find_or_add_vertex(g, data);
}

void graph_remove_vertex(struct graph* g, void* data)
{
    struct vertex* vertex;

    lazy_init(g);
    vertex = hashtable_get(&g->vertices, data);
    if (!vertex)
        return;
    
	for (;;)
	{
		struct vertex* other;
		struct edge* e = set_pop(&vertex->edges);
		if (!e)
			break;

		other = e->v1;
		if (other == vertex)
			other = e->v2;

		set_remove(&other->edges, e);
		other->degree--;
		free(set_remove(&g->edges, e));
	}

	set_reset(&vertex->edges);
    hashtable_remove(&g->vertices, data);
	free(vertex);
}

int graph_get_vertex_degree(struct graph* g, void* data)
{
    struct vertex* vertex;

    lazy_init(g);
    vertex = hashtable_get(&g->vertices, data);
    assert(vertex);
    return vertex->degree;
}

void* graph_next_vertex(struct graph* g, struct vertex_iterator* it)
{
    struct vertex* vertex = hashtable_next(&g->vertices, &it->hit);
    if (vertex)
    {
        it->data = vertex->data;
        return vertex->data;
    }
    else
        return NULL;
}

bool graph_next_edge(struct graph* g, struct edge_iterator* it)
{
    struct edge* e = set_next(&g->edges, &it->sit);
    if (e)
    {
        it->left = e->v1->data;
        it->right = e->v2->data;
        return true;
    }
    else
    {
        it->left = it->right = NULL;
        return false;
    }
}
