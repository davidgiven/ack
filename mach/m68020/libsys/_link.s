.define __link
.extern __link
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__link:		move.l #0x9,d0
		jmp (callc)
