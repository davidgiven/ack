.define _chmod
.extern _chmod
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_chmod:		move.l #0xF,d0
		jmp (callc)
