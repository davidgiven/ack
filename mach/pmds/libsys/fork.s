.define _fork
.extern _fork
_fork:		trap #0
.short 0x2
		bra 3f
		rts
3:
		bcc 2f
		jmp cerror
2:
		clr.l d0
		rts
