.define _ftime
.extern _ftime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_ftime:		clr.l -(sp)
		jsr _time
		lea 4(sp),sp
		move.l 4(sp),a0
		move.l d0,(a0)
		clr.l 4(a0)
		clr.w 8(a0)
		clr.l d0
		rts
