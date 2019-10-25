/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*
 * Include "mach.h", then if "mach.h" defines MACH_OPTIONS, also
 * declare mach_option(), a machine dependent function.
 */

#include "mach.h"

#ifdef MACH_OPTIONS
void mach_option(char *);
#endif
