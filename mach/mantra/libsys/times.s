.define _times
.extern _times
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_times:		move.l #0x2B,d0
		move.l 4(sp),a0
		trap #0
		rts
