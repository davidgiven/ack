.define _chroot
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _chroot
_chroot:			trap #0
.data2	0x3D
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
