.define _chroot
.extern _chroot
.text
_chroot:	move.w #0x3D,d0
		move.l 4(sp),a0
		jmp callc
