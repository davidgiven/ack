.define __fcntl
.extern __fcntl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__fcntl:		move.l #0x3E,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		jmp callc
