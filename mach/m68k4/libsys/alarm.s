.define _alarm
.extern _alarm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_alarm:		clr.l d0
		move.l 4(sp),a0
		move.w #0x1B,d0
		trap #0
		rts
