.define _stat
.extern _stat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_stat:		move.w #0x12,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jmp callc
