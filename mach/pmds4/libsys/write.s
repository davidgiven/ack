.define _write
.extern _write
_write:		trap #0
.short	0x4
			bcc	1f
			jmp	cerror
1:
			rts
