.define _mknod
.extern _mknod
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_mknod:		move.l #0xE,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		jmp callc
