.define _dup
.define _dup2
.extern _dup
.text
.extern _dup2
_dup2:		or.l	#64,4(sp)

_dup:			trap #0
.short	0x29
			bcc	1f
			jmp	cerror
1:
			rts
