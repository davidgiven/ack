.define _setuid
.extern _setuid
.text
_setuid:		trap #0
.short	0x17
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
