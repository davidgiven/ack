.define _chdir
.extern _chdir
_chdir:			trap #0
.short	0xC
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
