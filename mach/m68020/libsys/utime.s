.define _utime
.extern _utime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_utime:		move.l #0x1E,d0
		jmp (callc)
