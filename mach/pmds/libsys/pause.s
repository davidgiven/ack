.define _pause
.extern _pause
_pause:		trap #0
.short 29
		bcc 1f
		jmp cerror
1:
		rts
