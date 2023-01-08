#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! .sys_dpmidos: simulate a call to real-mode int 0x21 with the current
! 16-bit register values.

.extern .sys_dpmidos
.sys_dpmidos:
	movzx ebx, bx
	or ebx, 0x210000
	callf (interrupt_ptr)
	ret
