.define _mount
.extern _mount
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_mount:		move.l #0x15,d0
		jmp (callc)
