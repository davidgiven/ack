.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __cleanup
.extern __cleanup
__cleanup:
	push	bp
	mov	bp,sp
mov sp,bp
pop bp
ret
