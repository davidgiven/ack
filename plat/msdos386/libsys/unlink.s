#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! Remove a file.

.define _unlink
_unlink:
	! Copy filename to transfer buffer.

	mov eax, 4(esp)
	call .sys_scpyout

	! Make the DOS call.

	xchg edx, eax
	movb ah, 0x41
	call .sys_dpmidos

	jmp .sys_zret
