.sect .text; .sect .rom; .sect .data; .sect .bss
.define _geteuid
.sect .text
_geteuid:
	mov	ax,24
	callf	0x7:0x0
	jb	1f
	xchg	dx,ax
	ret
1:
	jmp	cerror
