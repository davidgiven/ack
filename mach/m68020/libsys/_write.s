.define __write
.extern __write
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__write:		move.l #0x4,d0
		jmp (call)
