.define _read
.extern _read
_read:		trap #0
.short	0x3
			bcc	1f
			jmp	cerror
1:
			rts
