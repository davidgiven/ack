.define __read
.extern __read
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__read:		move.l #0x3,d0
		jmp (call)
