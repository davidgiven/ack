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

! Get the current system time from MS-DOS.

.define __sys_gettime
__sys_gettime:
	int 3
	movb ah, 0x2c
	int 0x21
	mov bx, sp
	mov bx, 2(bx)
	mov (bx), cx
	mov 2(bx), dx
	ret
