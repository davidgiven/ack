.define _setuid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _setuid
.sect .text
_setuid:		trap #0
.data2	0x17
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
