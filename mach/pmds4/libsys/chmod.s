.define _chmod
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _chmod
_chmod:		trap #0
.data2	0xF
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
