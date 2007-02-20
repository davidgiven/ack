#
! $Source$
! $State$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

! Reads a single byte.

.define __sys_rawread
__sys_rawread:
	xorb ah, ah
	int 0x16
	xorb ah, ah
	ret
	