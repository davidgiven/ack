.define _pipe
.extern _pipe
_pipe:		
		move.w #0x2A,d0
		trap #0
		bcc 1f
		jmp cerror
1:
		move.l 4(sp),a0
		move.w d0,(a0)+
		move.w d1,(a0)
		clr.l d0
		rts
