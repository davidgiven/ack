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

#define TYPE_ATTRS \
    (burm_int_ATTR | burm_long_ATTR | burm_float_ATTR | burm_double_ATTR)

#include "ircodes-dyn.h"

#endif

/* vim: set sw=4 ts=4 expandtab : */

