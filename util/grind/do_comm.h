#ifndef DO_COMM_H
#define DO_COMM_H

extern void enterlog(p_tree p);
extern int setstop(p_tree p, int kind);
extern int settrace(p_tree p, int kind);
extern void free_dump(p_tree p);

#endif
