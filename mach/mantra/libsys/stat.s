.define _stat
.extern _stat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_stat:		move.l #0x12,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jsr callc
		tst.l d0
		blt 1f
		move.l 4(sp),a0
		add.l #30,a0
		lea 2(a0),a1
		move.w -(a0),-(a1)
		move.w -(a0),-(a1)
		move.w -(a0),-(a1)
		move.w -(a0),-(a1)
		move.w -(a0),-(a1)
		move.w -(a0),-(a1)
		move.w -(a0),-(a1)
		move.w -(a0),-(a1)
1:
		rts
