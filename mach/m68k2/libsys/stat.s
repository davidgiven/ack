.define _stat
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _stat
.sect .text
_stat:		move.w #0x12,d0
		move.l 4(sp),a0
		move.w 8(sp),d1
		ext.l  d1
		jmp callc
