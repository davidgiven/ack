.define _mount
.extern _mount
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_mount:		move.w #0x15,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		jmp callc
