.define _acct
.extern _acct
_acct:			trap #0
.short	0x33
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
