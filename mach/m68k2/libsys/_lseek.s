.define __lseek
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern __lseek
.sect .text
__lseek:		move.w #0x13,d0
		move.w 4(sp),a0
		move.l 6(sp),d1
		move.w 10(sp),a1
		jmp call
