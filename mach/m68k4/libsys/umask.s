.define _umask
.extern _umask
.text
_umask:		move.w #0x3C,d0
		move.l 4(sp),a0
		jmp call
