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

! Remove a file.

.define _unlink
_unlink:
	mov bx, sp
	mov dx, 2(bx)
	movb ah, 0x41
	int 0x21
	jmp .sys_zret
