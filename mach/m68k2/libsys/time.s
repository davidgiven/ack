.define _time
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _time
.sect .text
_time:		move.w #0xD,d0
		trap #0
		tst.l 4(sp)
		beq 1f
		move.l 4(sp),a0
		move.l d0,(a0)
1:
		rts
