.define __close
.extern __close
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__close:		move.l #0x6,d0
		jmp (callc)
