.sect .text; .sect .rom; .sect .data; .sect .bss
.define _getppid
.sect .text
_getppid:
	mov	ax,20
	callf	0x7:0x0
	jb	1f
	xchg	dx,ax
	ret
1:
	jmp	cerror
