.define __ftime
.extern __ftime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__ftime:		move.l #0x23,d0
		jmp (callc)
