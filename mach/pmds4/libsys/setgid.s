.define _setgid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _setgid
.sect .text
_setgid:		trap #0
.data2	0x2E
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
