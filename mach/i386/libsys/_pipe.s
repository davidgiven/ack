.sect .text; .sect .rom; .sect .data; .sect .bss
.define __pipe
.sect .text
__pipe:
	mov	ax,42
	callf	0x7:0x0
	jae	1f
	jmp	cerror
1:
	shl	dx,0x10
	or	eax,edx
	ret
