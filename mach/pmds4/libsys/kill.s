.define _kill
.extern _kill
_kill:		trap #0
.short	0x25
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
