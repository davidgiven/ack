/*
mach.h
*/

#ifndef MACH_H
#define MACH_H

#define con_str B_constr
#define rom_str B_romstr
#define gen_str B_genstr

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
