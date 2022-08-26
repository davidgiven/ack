#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! Say whether a file exists with the given name.

.define __sys_exists
__sys_exists:
	mov eax, 4(esp)
	call .sys_scpyout
	jc 9f
	xchg edx, eax
	mov eax, 0x4300
	call .sys_dpmidos
9:
	sbb eax, eax
	inc eax
	ret
