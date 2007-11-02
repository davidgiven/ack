#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"
	
.sect .rom

! Contains a handy double-precision 0x80000000.

.define .fd_80000000
.fd_80000000:
	!float 2.147483648e+9 sz 8
	.data1 0101,0340,00,00,00,00,00,00
