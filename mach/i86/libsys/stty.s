.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _stty
.extern _stty, _ioctl
_stty:
	push	bp
	mov	bp,sp
push 6(bp)
mov ax,0x5402
push ax
push 4(bp)
call _ioctl
add sp,6
mov sp,bp
pop bp
ret
