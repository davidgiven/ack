.define _umount
.extern _umount
_umount:		trap #0
.short	0x16
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
