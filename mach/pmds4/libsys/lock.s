.define _lock
.extern _lock
_lock:		trap #0
.short	0x35
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
