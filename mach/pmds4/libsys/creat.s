.define _creat
.extern _creat
_creat:			trap #0
.short	0x8
			bcc	1f
			jmp	cerror
1:
			rts
