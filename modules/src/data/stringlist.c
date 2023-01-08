/** @file
 * String list implementation.
 *
 */
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

void stringlist_free(struct stringlist* list, int freedata)
{
	struct stringfragment* f = list->first;

	while (f)
	{
		struct stringfragment* next = f->next;
		if (freedata)
		{
			free((void*) f->data);
			f->data = NULL;
		}
		free(f);
		f = next;
	}
}

void stringlist_init(struct stringlist* list)
{
	list->first = NULL;
	list->last = NULL;
}


const char* stringlist_get(struct stringlist *list, int index)
{
	struct stringfragment* f = list->first;
	int i = 0;

	while (f)
	{
		if (i == index)
		{
			return f->data;
		}
		f = f->next;
		i++;

	}
	return NULL;

}

int stringlist_count(struct stringlist *list)
{
	struct stringfragment* f = list->first;
	int i = 0;

	while (f)
	{
		f = f->next;
		i++;
	}
	return i;
}

/* vim: set sw=4 ts=4 expandtab : */


