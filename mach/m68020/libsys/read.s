.define _read
.extern _read
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_read:		move.l #0x3,d0
		jmp (call)
