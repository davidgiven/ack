#include "tables.h"
#ifdef USE_SHC
#include <stdio.h>
#include "types.h"
#include "param.h"
#include "label.h"

static label_p label_list = NULL;
extern char *myalloc();

add_label(num, height, flth)
{
	register label_p lbl = NULL;

	if (height <= 0) return;
	if (flth != TRUE && flth != FALSE)
	    fatal("incorrect value for fallthrough");

	lbl = (label_p) myalloc(sizeof(label_t));
	lbl->lb_next = label_list;
	lbl->lb_number = num;
	lbl->lb_height = height;
	lbl->lb_fallthrough = flth;
	label_list = lbl;
}

label_p get_label(num)
register word num;
{
	register label_p tmp = label_list;

	while (tmp != NULL) {
		if (tmp->lb_number == num) return tmp;
		tmp = tmp->lb_next;
	}
	return NULL;
}

kill_labels()
{
	label_p tmp;

	while((tmp = label_list) != NULL) {
	    label_list = label_list->lb_next;
	    myfree((char *)tmp);
	}
}
#endif
