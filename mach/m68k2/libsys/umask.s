.define _umask
.extern _umask
.text
_umask:		move.w #0x3C,d0
		move.w 4(sp),a0
		jmp call
