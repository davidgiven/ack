.define _stty
.extern _stty
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_stty:		move.l #0x21, d0
		jmp (call)
