.define __gtty
.extern __gtty
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__gtty:		move.l #0x20, d0
		jmp (call)
