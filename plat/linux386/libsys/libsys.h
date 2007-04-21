/* $Source$
 * $State$
 * $Revision$
 */

#ifndef LIBSYS_H
#define LIBSYS_H

#include "syscalls.h"

typedef unsigned long quad;

extern quad _syscall(int op, quad p1, quad p2, quad p3); 

#endif
