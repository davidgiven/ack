.define _unlink
.extern _unlink
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_unlink:	move.l #0xA,d0
		jmp (callc)
