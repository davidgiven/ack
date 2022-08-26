#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! Get the current system date from MS-DOS.

.define __sys_getdate
__sys_getdate:
	movb ah, 0x2a
	int 0x21
	mov ebx, esp
	mov ebx, 4(ebx)
	o16 mov 0(ebx), dx
	o16 mov 2(ebx), cx
	ret
