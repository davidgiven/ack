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

__NR_execve = 11

! Perform a execl by invoking Linux's execve system call.

.define _execl

_execl:
	lea eax, 8(esp)
	push (_environ)			! envp
	push eax			! argv
	push -4(eax)			! pathname
	push __NR_execve
	call __syscall
	add esp, 16
	ret
