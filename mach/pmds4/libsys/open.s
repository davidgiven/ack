.define _open
.extern _open
_open:		trap #0
.short	0x5
			bcc	1f
			jmp	cerror
1:
			rts
