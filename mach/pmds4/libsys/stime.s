.define _stime
.extern _stime
.text
_stime:		trap #0
.short	0x19
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
