.define _lseek
.extern _lseek
_lseek:		trap #0
.short	0x13
			bcc	1f
			jmp	cerror
1:
			rts
