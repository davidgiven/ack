.define cerror
.extern cerror
cerror:			move.l d0,_errno
			move.l #-1,d0
			rts
.bss
_errno:
			.space 4
.text
