.define __open
.extern __open
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__open:		move.l #0x5,d0
		jmp (call)
