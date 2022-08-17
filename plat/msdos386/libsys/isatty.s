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
	mov ebx, sp
	mov ebx, 4(ebx)
	o16 mov ax, 0x4400
	int 0x21
	jc error
	testb dl, dl
	jz not_tty
	mov eax, 1
	ret
not_tty:
	mov (_errno), 25		! ENOTTY
not_tty_2:
	xor eax, eax
	ret
error:
	push eax
	call __sys_seterrno
	pop ecx
	jmp not_tty_2
