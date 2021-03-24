#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

! .sys_zret: if the carry flag is set, then set `errno' from the DOS error
! code in ax, and return -1.  If the carry flag is clear, just return zero.
!
! .sys_axret: if the carry flag is set, then set `errno' from the DOS error
! code in ax, and return -1.  If the carry flag is clear, just return ax as
! a return value.

.extern .sys_zret
.extern .sys_axret
.sys_zret:
	jc error
	xor ax, ax
no_error:
	ret
.sys_axret:
	jnc no_error
error:
	push ax
	call __sys_seterrno
	pop cx
	ret
