.define __open
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __open
__open:		trap #0
.data2	0x5
			bcc	1f
			jmp	cerror
1:
			rts
