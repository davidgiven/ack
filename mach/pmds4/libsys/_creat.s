.define __creat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __creat
__creat:			trap #0
.data2	0x8
			bcc	1f
			jmp	cerror
1:
			rts
