.define __creat
.extern __creat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__creat:		move.l #0x8,d0
		jmp (call)
