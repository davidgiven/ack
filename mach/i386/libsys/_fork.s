.sect .text; .sect .rom; .sect .data; .sect .bss
.define __fork
.sect .text
__fork:
	mov	ax,2
	callf	0x7:0x0
	jb	1f
	and	dx,dx
	jne	2f
	ret
1:
	jmp	cerror
2:
	xor	ax,ax
	ret
