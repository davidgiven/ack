.define _dup2
.extern _dup2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_dup2:		move.w #0x29,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		add.l #0x40,a0
		jmp call
