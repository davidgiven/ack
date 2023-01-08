#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! Open an existing file.

.define __sys_rawopen
__sys_rawopen:
	! Copy filename to transfer buffer.

	mov eax, 4(esp)
	call .sys_scpyout

	! Make the DOS call.

	xchg edx, eax
	movb ah, 0x3d
	movb al, 8(esp)
	call .sys_dpmidos

	jmp .sys_axret
