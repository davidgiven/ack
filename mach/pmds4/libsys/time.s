.define _time
.define _ftime
.extern _time
_time:
	trap #0
.short	0xD
	tst.l 4(sp)
	beq 1f
	move.l 4(sp),a0
	move.l d0,(a0)
1:
	rts
.extern _ftime
_ftime:
	trap #0
.short 0x23
	bcc 1f
	jmp cerror
1:
	clr.l d0
	rts
