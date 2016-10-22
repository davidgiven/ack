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

#include "ircodes-dyn.h"

#endif

/* vim: set sw=4 ts=4 expandtab : */

