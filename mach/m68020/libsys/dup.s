.define _dup
.extern _dup
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_dup:		move.l #0x29,d0
		jmp (call)
