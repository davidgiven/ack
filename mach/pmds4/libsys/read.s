.define _read
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _read
_read:		trap #0
.data2	0x3
			bcc	1f
			jmp	cerror
1:
			rts
