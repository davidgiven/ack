.define _chdir
.extern _chdir
.text
_chdir:		move.w #0xC,d0
		move.l 4(sp),a0
		jmp callc
