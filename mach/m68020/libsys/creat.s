.define _creat
.extern _creat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_creat:		move.l #0x8,d0
		jmp (call)
