.define _nice
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _nice
.sect .text
_nice:		trap #0
.data2	0x22
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
