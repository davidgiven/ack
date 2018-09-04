#
! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

#define EINVAL 22

! Perform a Linux system call.

.define __syscall
__syscall:
	syscall 0

