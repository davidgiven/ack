.define _kill
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _kill
_kill:		trap #0
.data2	0x25
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
