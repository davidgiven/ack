.define __dup
.extern __dup
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__dup:		move.l #0x29,d0
		jmp (call)
