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

! Get the current system date from MS-DOS.

.define __sys_getdate
__sys_getdate:
	movb ah, 0x2a
	int 0x21
	mov bx, sp
	mov bx, 2(bx)
	mov (bx), dx
	mov 2(bx), cx
	ret
