.define _ioctl
.extern _ioctl
_ioctl:		trap #0
.short	0x36
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
