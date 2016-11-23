#include <stdlib.h>
#include "stringlist.h"

void stringlist_add(struct stringlist* list, const char* data)
{
	struct stringfragment* f = calloc(1, sizeof *f);
	f->data = data;

	if (!list->last)
		list->first = list->last = f;
	else
	{
		list->last->next = f;
		list->last = f;
	}
}

void stringlist_addall(struct stringlist* list, struct stringlist* src)
{
	struct stringfragment* f = src->first;

	while (f)
	{
		stringlist_add(list, f->data);
		f = f->next;
	}
}

void stringlist_free(struct stringlist* list)
{
	struct stringfragment* f = list->first;

	while (f)
	{
		struct stringfragment* next = f->next;
		free(f);
		f = next;
	}
}

/* vim: set sw=4 ts=4 expandtab : */


