.define _setgid
.extern _setgid
.text
_setgid:		trap #0
.short	0x2E
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
