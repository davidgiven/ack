.define _utime
.extern _utime
_utime:			trap #0
.short	0x1E
			bcc	1f
			jmp	cerror
1:
			rts
