.define _unlink
.extern _unlink
_unlink:		trap #0
.short	0xA
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
