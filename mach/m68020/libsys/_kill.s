.define __kill
.extern __kill
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__kill:		move.l #0x25,d0
		jmp (callc)
