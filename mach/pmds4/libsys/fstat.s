.define _fstat
.extern _fstat
_fstat:			trap #0
.short	0x1C
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
