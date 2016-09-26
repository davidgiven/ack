#ifndef IRCODES_H
#define IRCODES_H

enum
{
	IRF_SIZED = 1,
};

enum ir_type
{
	IRT_UNSET = 0,
	IRT_INT,
	IRT_FLOAT,
	IRT_ANY
};

struct ir_data
{
	const char* name;
	int flags;
	enum ir_type returntype;
	enum ir_type lefttype;
	enum ir_type righttype;
};

extern const struct ir_data ir_data[];

#include "ircodes-dyn.h"

#endif

/* vim: set sw=4 ts=4 expandtab : */

