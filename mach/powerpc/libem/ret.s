#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"

.sect .text
	
! Standard boilerplate for returning from functions.

.define .ret
.ret:
	lwz r0, 4(fp)
	mtspr lr, r0
	lwz r0, 0(fp)            ! our stack frame becomes invalid as soon as...
	addi sp, fp, 8           ! ...we change sp
	or fp, r0, r0
	bclr ALWAYS, 0, 0
