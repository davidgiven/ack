.define _umount
.extern _umount
.text
_umount:	move.w #0x16,d0
		move.l 4(sp),a0
		jmp callc
