.define _stime
.extern _stime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_stime:		move.l #0x19,d0
		move.l ([4,sp]),(4,sp)
		jmp (callc)
