.define _open
.extern _open
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_open:		move.l #0x5,d0
		jmp (call)
