.define _pipe
.extern _pipe
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_pipe:		
		move.l #0x2A,d0
		trap #0
		bcc 1f
		jmp cerror
1:
		move.l 4(sp),a0
		move.l d0,(a0)+
		move.l d1,(a0)
		clr.l d0
		rts
