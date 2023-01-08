#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! .sys_zret: if the carry flag is set, then set `errno' from the DOS error
! code in ax, and return an int -1.  If the carry flag is clear, just
! return an int zero.
!
! .sys_axret: if the carry flag is set, then set `errno' from the DOS error
! code in ax, and return a shortword -1.  If the carry flag is clear, just
! return eax := ax as a return value.
!
! .sys_dxaxret: same as .sys_axret, but return -1 or eax := dx:ax as a
! return value.
!
! .sys_toolongret: a file name is too long: set `errno' to E2BIG, and return
! -1.

.extern .sys_zret
.extern .sys_axret
.extern .sys_dxaxret
.extern .sys_toolongret
.sys_zret:
	jc error
	xor eax, eax
no_error:
	ret

.sys_axret:
	jc error
	movzx eax, ax
	ret

.sys_dxaxret:
	jc error
	shl edx, 16
	o16 mov dx, ax
	mov eax, edx
	ret

.sys_toolongret:
	mov (_errno), 7			! E2BIG
	or eax, -1
	ret

error:
	movzx eax, ax
	push eax
	call __sys_seterrno
	pop ecx
	ret
