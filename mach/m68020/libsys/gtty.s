.define _gtty
.extern _gtty
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_gtty:		move.l #0x20, d0
		jmp (call)
