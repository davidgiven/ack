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
! code in ax, and return a shortword -1.  If the carry flag is clear, just
! return a shortword zero.
!
! .sys_axret: if the carry flag is set, then set `errno' from the DOS error
! code in ax, and return a shortword -1.  If the carry flag is clear, just
! return ax as a return value.
!
! .sys_dxaxret: same as .sys_axret, but return a longword -1 or dx:ax as a
! return value.

.extern .sys_zret
.extern .sys_axret
.extern .sys_dxaxret
.sys_zret:
	jc error
	xor ax, ax
no_error:
	ret
.sys_axret:
.sys_dxaxret:
	jnc no_error
error:
	push ax
	call __sys_seterrno
	pop cx
	ret
