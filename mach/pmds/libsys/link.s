.define _link
.extern _link
_link:			trap #0
.short	0x9
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
