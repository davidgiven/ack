.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __gtty
.extern __gtty, __ioctl
__gtty:
	push	bp
	mov	bp,sp
push 6(bp)
mov ax,0x5401
push ax
push 4(bp)
call __ioctl
add sp,6
mov sp,bp
pop bp
ret
