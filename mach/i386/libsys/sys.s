.sect .text; .sect .rom; .sect .data; .sect .bss
.define sysx, syscal, cerror, _errno

.sect .bss
	.comm	_errno, 4

.sect .text

syscal:
	callf	0x7:0x0
	jb	cerror
	ret

sysx:
	callf	0x7:0x0
	jb	cerror
	xor	ax,ax
	ret
cerror:
	mov	(_errno),ax
	mov	ax,-1
	ret

