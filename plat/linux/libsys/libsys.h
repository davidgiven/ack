/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/libsys.h,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#ifndef LIBSYS_H
#define LIBSYS_H

#include "syscalls.h"

typedef unsigned long quad;

extern quad _syscall(int op, quad p1, quad p2, quad p3); 

#endif
