.define __wait
.extern __wait
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__wait:		move.w	#0x7,d0
		move.l	4(sp),a0
		trap #0
		bcs cerror
		tst.l 4(sp)
		beq 1f
		move.l 4(sp),a0
		move.l d1,(a0)
1:
		rts
