.define _ptrace
.extern _ptrace
.extern _errno
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_ptrace:	move.w #0x1A,d0
		move.l 16(sp),a0
		move.l 12(sp),d1
		move.l 8(sp),a1
		move.l 4(sp),d2
		clr.l  _errno
		trap #0
		rts
