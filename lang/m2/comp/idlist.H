/* $Header$ */

#include <alloc.h>

/*	Structure to link idf structures together
*/
struct id_list {
	struct id_list *next;
	struct idf *id_ptr;
};

/* ALLOCDEF "id_list" */
