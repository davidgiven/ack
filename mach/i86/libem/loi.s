.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .loi

	! #bytes in cx
	! source address in si
.loi:
	pop     bx
	mov     dx,cx
	sar     cx,1
	jnb     1f
	xorb    ah,ah
	lodsb
	push    ax
	jmp     bx
1:
	sub     sp,dx
	mov     di,sp
	rep movs
	jmp     bx
