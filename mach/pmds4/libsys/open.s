.define _open
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _open
_open:		trap #0
.data2	0x5
			bcc	1f
			jmp	cerror
1:
			rts
