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

! Open an existing file.

.define __sys_rawopen
__sys_rawopen:
	int 3
	movb ah, 0x3d
	mov bx, sp
	mov dx, 2(bx)
	movb al, 4(bx)
	int 0x21
	jmp .sys_axret
