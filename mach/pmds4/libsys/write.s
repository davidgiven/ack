.define _write
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _write
_write:		trap #0
.data2	0x4
			bcc	1f
			jmp	cerror
1:
			rts
