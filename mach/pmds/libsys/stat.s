.define _stat
.extern _stat
_stat:			trap #0
.short	0x12
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
