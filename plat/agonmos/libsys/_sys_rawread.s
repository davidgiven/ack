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

! Reads a single byte.

.define __sys_rawread
__sys_rawread:
	ld de, 0
	xor a
	rst 0x08
	ld e, a
	ret

