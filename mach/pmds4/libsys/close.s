.define _close
.extern _close
_close:		trap #0
.short	0x6
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
