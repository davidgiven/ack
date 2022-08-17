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

! Say whether a given file descriptor number refers to a valid open file
! descriptor.

.define __sys_isopen
__sys_isopen:
	int 3
	mov bx, sp
	mov bx, 2(bx)
	mov ax, 0x4400
	int 0x21
	sbb ax, ax
	inc ax
	ret
