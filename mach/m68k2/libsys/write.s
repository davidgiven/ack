.define _write
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _write
.sect .text
_write:		move.w #0x4,d0
		move.w 4(sp),a0
		move.l 6(sp),d1
		move.w 10(sp),a1
		jmp call
