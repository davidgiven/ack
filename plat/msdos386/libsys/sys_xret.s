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
! code in ax, and return an int -1.  If the carry flag is clear, just
! return an int zero.
!
! .sys_axret: if the carry flag is set, then set `errno' from the DOS error
! code in ax, and return a shortword -1.  If the carry flag is clear, just
! return ax as a return value.
!
! .sys_dxaxret: same as .sys_axret, but return -1 or eax := dx:ax as a
! return value.

.extern .sys_zret
.extern .sys_axret
.extern .sys_dxaxret
.sys_zret:
	jc error
	xor eax, eax
no_error:
	ret

.sys_axret:
	jc error
	ret

.sys_dxaxret:
	jc error
	shl edx, 16
	o16 mov dx, ax
	mov eax, edx
	ret

error:
	movzx eax, ax
	push eax
	call __sys_seterrno
	pop ecx
	ret
