.define _unlink
.extern _unlink
.text
_unlink:	move.w #0xA,d0
		move.l 4(sp),a0
		jmp callc
