.define _access
.extern _access
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_access:	move.l #0x21,d0
		jmp (callc)
