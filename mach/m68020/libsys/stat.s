.define __stat
.extern __stat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__stat:		move.l #0x12,d0
		jmp (callc)
