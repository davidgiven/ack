.define __execve
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern __execve
.sect .text
__execve:	move.w #0x3B,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		jmp calle
