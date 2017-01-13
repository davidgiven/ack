#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "heap.h"

struct heap_node
{
    struct heap_node* next;
    char data[1];
};

void* heap_alloc(struct heap* heap, size_t nmemb, size_t size)
{
    size_t bytes = nmemb * size;
    struct heap_node* node = calloc(1, sizeof(*node) + bytes - 1);
    if (!node)
        return NULL;

    node->next = heap->next;
    heap->next = node;
    return node->data;
}

void heap_free(struct heap* heap)
{
    while (heap->next)
    {
        struct heap_node* old = heap->next;
        heap->next = old->next;
        free(old);
    }
}
