.define _setuid
.extern _setuid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_setuid:	move.l #0x17,d0
		jmp (callc)
