.sect .text; .sect .rom; .sect .data; .sect .bss
.define __getpid
.sect .text
__getpid:
	mov	ax,20
	call	syscal
	ret
