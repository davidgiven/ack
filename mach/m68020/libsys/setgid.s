.define _setgid
.extern _setgid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_setgid:	move.l #0x2E,d0
		jmp (callc)
