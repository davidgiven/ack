.define _mknod
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _mknod
.sect .text
_mknod:		move.w #0xE,d0
		move.l 4(sp),a0
		move.w 8(sp),d1
		ext.l  d1
		move.w 10(sp),a1
		jmp callc
