.define __creat
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern __creat
.sect .text
__creat:		move.w #0x8,d0
		move.l 4(sp),a0
		move.w 8(sp),d1
		ext.l  d1
		jmp call
