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

! Say whether a file exists with the given name.

.define __sys_exists
__sys_exists:
	mov bx, sp
	mov dx, 2(bx)
	mov ax, 0x4300
	int 0x21
	sbb ax, ax
	inc ax
	ret
