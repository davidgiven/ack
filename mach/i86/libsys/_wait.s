.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __wait
.extern __wait, cerror
__wait:
	mov bx,sp
	mov ax,2	! void info about unused memory
	! Should be 0 according to /usr/include/sys.s, but
	! that doesn't work
	push ax
	push 2(bx)
	push ax		! dummy return address
	int 0x87
	jb 9f
	add sp,6
	ret
9:	add sp,6
	jmp cerror
