.define _nice
.extern _nice
.text
_nice:		trap #0
.short	0x22
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
