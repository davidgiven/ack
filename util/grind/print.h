#ifndef PRINT_H
#define PRINT_H

struct type;
extern void print_val(
    struct type* tp, /* type of value to be printed */
    long tp_sz, /* size of object to be printed */
    char* addr, /* address to get value from */
    int compressed, /* for parameter lists */
    int indent, /* indentation */
    char* format /* format given or 0 */
);

extern void print_params(p_type tp, t_addr AB, int static_link);

#endif
