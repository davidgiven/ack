.define _mknod
.extern _mknod
.text
_mknod:		trap #0
.short	0xE
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
