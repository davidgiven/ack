.define _wait
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _wait
_wait:		move.w	#0x7,d0
		move.l	4(sp),a0
		trap #0
		bcs cerror
		tst.l 4(sp)
		beq 1f
		move.l 4(sp),a0
		move.w d1,(a0)
1:
		rts
