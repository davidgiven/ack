.define _link
.extern _link
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_link:		move.l #0x9,d0
		jmp (callc)
