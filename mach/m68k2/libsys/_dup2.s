.define __dup2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __dup2
.sect .text
__dup2:		move.w #0x29,d0
		move.w 4(sp),a0
		move.w 6(sp),d1
		ext.l  d1
		add.l #0x40,a0
		jmp call
