.define _kill
.extern _kill
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_kill:		move.l #0x25,d0
		jmp (callc)
