.define __unlink
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __unlink
__unlink:		trap #0
.data2	0xA
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
