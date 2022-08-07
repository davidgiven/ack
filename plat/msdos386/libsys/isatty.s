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

! Say whether a given file descriptor number refers to a terminal.

.define _isatty
_isatty:
	mov bx, sp
	mov bx, 2(bx)
	mov ax, 0x4400
	int 0x21
	jc error
	testb dl, dl
	jz not_tty
	mov ax, 1
	ret
not_tty:
	mov (_errno), 25		! ENOTTY
not_tty_2:
	xor ax, ax
	ret
error:
	push ax
	call __sys_seterrno
	pop cx
	jmp not_tty_2
