.define _stat
.extern _stat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_stat:		move.l #0x12,d0
		jmp (callc)
