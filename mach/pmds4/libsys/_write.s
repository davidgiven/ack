.define __write
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __write
__write:		trap #0
.data2	0x4
			bcc	1f
			jmp	cerror
1:
			rts
