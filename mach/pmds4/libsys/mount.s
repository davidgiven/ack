.define _mount
.extern _mount
.text
_mount:		trap #0
.short	0x15
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
