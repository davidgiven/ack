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

! Create or truncate a file.

.define __sys_rawcreat
__sys_rawcreat:
	movb ah, 0x3c
	mov bx, sp
	mov dx, 2(bx)
	mov cx, 4(bx)
	int 0x21
	jmp .sys_axret
