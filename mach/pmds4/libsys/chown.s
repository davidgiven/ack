.define _chown
.extern _chown
.text
_chown:		trap #0
.short	0x10
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
