/*
mach.h
*/

#ifndef MACH_H
#define MACH_H

#include "ctype.h"
#include "mach_em.h"
#include "push_pop.h"
#include "ms_reg.h"
#include "misc.h"

#if RESOLV_debug
	#define debug 0
#else
	extern int debug;
#endif

#endif /* MACH_H */
