.define _times
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _times
.sect .text
_times:		move.w #0x2B,d0
		move.l 4(sp),a0
		trap #0
		rts
