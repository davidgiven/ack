#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! Create or truncate a file.

.define __sys_rawcreat
__sys_rawcreat:
	! Copy filename to transfer buffer.

	mov eax, 4(esp)
	call .sys_scpyout
	jc 9f

	! Make the DOS call.

	xchg edx, eax
	movb ah, 0x3c
	movb al, 8(esp)
	call .sys_dpmidos

	jmp .sys_axret

9:
	jmp .sys_toolongret
