#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"
	
.sect .rom

! Contains a handy double-precision 0xFFFFFFFF.

.define .fd_FFFFFFFF
.fd_FFFFFFFF:	
	!float 4.294967295e+9 sz 8
	.data1 0101,0357,0377,0377,0377,0340,00,00
