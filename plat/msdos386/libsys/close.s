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

! Close a file.

.define _close
_close:
	mov ebx, esp
	mov ebx, 1*4(esp)
	movb ah, 0x3E
	int 0x21
	jmp .sys_zret
