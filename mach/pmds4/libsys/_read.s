.define __read
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __read
__read:		trap #0
.data2	0x3
			bcc	1f
			jmp	cerror
1:
			rts
