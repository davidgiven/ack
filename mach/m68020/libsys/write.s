.define _write
.extern _write
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_write:		move.l #0x4,d0
		jmp (call)
