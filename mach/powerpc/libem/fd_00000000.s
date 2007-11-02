#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"
	
.sect .rom

! Contains a handy double-precision zero. (Also works as a single-precision
! zero.)

.define .fd_00000000, .fs_00000000
.fd_00000000:
.fs_00000000:
	.data4 0x00000000
	.data4 0x00000000
