.define __wait
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __wait
__wait:		trap #0
.data2 0x7
		bcc 1f
		jmp cerror
1:
		tst.l 4(sp)
		beq 2f
		move.l 4(sp),a0
		move.l d1,(a0)
2:
		rts
