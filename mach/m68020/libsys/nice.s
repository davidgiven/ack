.define _nice
.extern _nice
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_nice:		move.l #0x22,d0
		jmp (call)
