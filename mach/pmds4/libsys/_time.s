.define _time
.define __ftime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _time
_time:
	trap #0
.data2	0xD
	tst.l 4(sp)
	beq 1f
	move.l 4(sp),a0
	move.l d0,(a0)
1:
	rts
.extern __ftime
__ftime:
	trap #0
.data2 0x23
	bcc 1f
	jmp cerror
1:
	clr.l d0
	rts
