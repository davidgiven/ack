.sect .text; .sect .rom; .sect .data; .sect .bss
.define __exit
.sect .text
__exit:
	mov	ax,1
	callf	0x7:0x0
	jmp	cerror
