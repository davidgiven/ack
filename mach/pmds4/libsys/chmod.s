.define _chmod
.extern _chmod
_chmod:		trap #0
.short	0xF
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
