.define _access
.extern _access
_access:		trap #0
.short	0x21
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
