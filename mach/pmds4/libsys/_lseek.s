.define __lseek
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __lseek
__lseek:		trap #0
.data2	0x13
			bcc	1f
			jmp	cerror
1:
			rts
