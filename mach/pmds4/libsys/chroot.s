.define _chroot
.extern _chroot
_chroot:			trap #0
.short	0x3D
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
