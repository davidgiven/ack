.define __unlink
.extern __unlink
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__unlink:	move.l #0xA,d0
		jmp (callc)
