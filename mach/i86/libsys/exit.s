.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _exit
.extern _exit, __cleanup, __exit
_exit:
	push	bp
	mov	bp,sp
call __cleanup
push 4(bp)
call __exit
pop si
mov sp,bp
pop bp
ret
