#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "bigraph.h"

struct edgenode
{
    struct edgenode* next;
    struct vertex* this;
    struct vertex* other;
};

struct vertex
{
    void* data;
    struct edgenode* connections;
    int degree;
};

static uint32_t edge_hash_function(void* key)
{
    struct edgenode* en = key;
    /* This will always return the same value, even if the two endpoints are swapped. */
    return standard_pointer_hash_function(en->this) ^ standard_pointer_hash_function(en->other);
}

static bool edge_comparison_function(void* key1, void* key2)
{
    struct edgenode* en1 = key1;
    struct edgenode* en2 = key2;

    return ((en1->this == en2->this) && (en1->other == en2->other))
        || ((en1->this == en2->other) && (en1->other == en2->this));
}

static void lazy_init(struct graph* g)
{
    g->edges.hashfunction = edge_hash_function;
    g->edges.cmpfunction = edge_comparison_function;
}

void graph_reset(struct graph* g)
{
    lazy_init(g);

    for (;;)
    {
        struct vertex* vertex = hashtable_pop(&g->vertices);
        if (!vertex)
            return;

        while (vertex->connections)
        {
            struct edgenode* next = vertex->connections->next;
            free(vertex->connections);
            vertex->connections = next;
        }

        free(vertex);
    }
}

bool graph_contains_vertex(struct graph* g, void* data)
{
    lazy_init(g);

    return hashtable_contains(&g->vertices, data);
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

static struct edgenode** find_edgep(struct vertex* v1, struct vertex* v2)
{
    struct edgenode** ep = &v1->connections;

    while (*ep)
    {
        if ((*ep)->other == v2)
            return ep;
        ep = &(*ep)->next;
    }

    return ep;
}

bool graph_contains_edge(struct graph* g, void* data1, void* data2)
{
    struct vertex* v1 = find_or_add_vertex(g, data1);
    struct vertex* v2 = find_or_add_vertex(g, data2);

	if (!v1 || !v2)
		return false;

	return *find_edgep(v1, v2) || *find_edgep(v2, v1);
}

static struct edgenode* add_edge(struct vertex* v1, struct vertex* v2)
{
    struct edgenode** ep = find_edgep(v1, v2);

    if (!*ep)
    {
        *ep = calloc(1, sizeof(struct edgenode));
        (*ep)->this = v1;
        (*ep)->other = v2;
        v1->degree++;
    }

    return *ep;
}

void graph_add_edge(struct graph* g, void* data1, void* data2)
{
    struct vertex* v1 = find_or_add_vertex(g, data1);
    struct vertex* v2 = find_or_add_vertex(g, data2);
    struct edgenode* e;
	struct edgenode template;

	if (v1 == v2)
		return;

	template.this = v1;
	template.other = v2;
	if (hashtable_contains(&g->edges, &template))
		return;

    add_edge(v1, v2);
    e = add_edge(v2, v1);

    hashtable_put(&g->edges, e, e);
}

static struct edgenode* remove_edge(struct vertex* v1, struct vertex* v2)
{
    struct edgenode** ep = find_edgep(v1, v2);

    if (*ep)
    {
        struct edgenode* old = *ep;
        *ep = (*ep)->next;
        v1->degree--;
        return old;
    }

    return NULL;
}

void graph_remove_edge(struct graph* g, void* data1, void* data2)
{
    struct vertex* v1 = find_or_add_vertex(g, data1);
    struct vertex* v2 = find_or_add_vertex(g, data2);
    struct edgenode* e1 = remove_edge(v1, v2);
    struct edgenode* e2 = remove_edge(v2, v1);

    assert(!e1 == !e2);

    if (e1)
    {
        /* e1 is a template; the actual object in the hashtable might actually
         * be e2 (as they compare the same). So, remove from the hashtable
         * before freeing anything. */
        hashtable_remove(&g->edges, e1);
        free(e1);
    }
    if (e2)
        free(e2);
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
    
    while (vertex->connections)
    {
        struct edgenode* next = vertex->connections->next;
        struct edgenode* e = remove_edge(vertex->connections->other, vertex);
        hashtable_remove(&g->edges, vertex->connections);
        free(e);
        free(vertex->connections);
        vertex->connections = next;
    }

    hashtable_remove(&g->vertices, data);
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
    struct edgenode* e = hashtable_next(&g->edges, &it->hit);
    if (e)
    {
        it->left = e->this->data;
        it->right = e->other->data;
        return true;
    }
    else
    {
        it->left = it->right = NULL;
        return false;
    }
}
