.define __fstat
.extern __fstat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__fstat:	move.l	#0x1C,d0
		jmp (callc)
