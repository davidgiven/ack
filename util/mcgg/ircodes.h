#ifndef IRCODES_H
#define IRCODES_H

enum
{
	IRF_SIZED = 1,
};

struct ir_data
{
	const char* name;
	int flags;
    char returntype;
    char lefttype;
    char righttype;
};

extern const struct ir_data ir_data[];

#define burm_int_CLASS 0
#define burm_long_CLASS 1
#define burm_float_CLASS 2
#define burm_double_CLASS 3

#define burm_int_ATTR (1U<<burm_int_CLASS)
#define burm_long_ATTR (1U<<burm_long_CLASS)
#define burm_float_ATTR (1U<<burm_float_CLASS)
#define burm_double_ATTR (1U<<burm_double_CLASS)

#define TYPE_ATTRS \
    (burm_int_ATTR | burm_long_ATTR | burm_float_ATTR | burm_double_ATTR)

#include "ircodes-dyn.h"

#endif

/* vim: set sw=4 ts=4 expandtab : */

