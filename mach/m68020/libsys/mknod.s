.define _mknod
.extern _mknod
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_mknod:		move.l #0xE,d0
		jmp (callc)
