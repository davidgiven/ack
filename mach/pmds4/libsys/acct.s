.define _acct
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _acct
_acct:			trap #0
.data2	0x33
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
