.define _ptrace
.extern _ptrace
_ptrace:		trap #0
.short	0x1A
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
