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

! Writes a single byte to the console.

.define __sys_rawwrite
.extern __sys_rawwrite

__sys_rawwrite:
	ld hl, 3
	add hl, sp
	ld a, (hl)
	rst 0x10
	ret
	
