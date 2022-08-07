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

! Rename a file.

.define _rename
_rename:
	mov bx, sp
	push di
	mov dx, 2(bx)
	mov di, 4(bx)
	movb ah, 0x56
	int 0x21
	pop di
	jmp .sys_zret
