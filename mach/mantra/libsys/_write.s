.define __write
.extern __write
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__write:		move.l #0x4,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		jmp call
