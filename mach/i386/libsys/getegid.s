.sect .text; .sect .rom; .sect .data; .sect .bss
.define _getegid
.sect .text
_getegid:
	mov	ax,47
	callf	0x7:0x0
	jb	1f
	xchg	dx,ax
	ret
1:
	jmp	cerror
