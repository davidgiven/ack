.define _creat
.extern _creat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_creat:		move.w #0x8,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jmp call
