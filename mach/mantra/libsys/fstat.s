.define _fstat
.extern _fstat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_fstat:		move.l	#0x1C,d0
		move.l	4(sp),a0
		move.l	8(sp),d1
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
