.define _link
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _link
_link:			trap #0
.data2	0x9
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
