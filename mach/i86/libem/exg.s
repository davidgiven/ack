.sect .text; .sect .rom; .sect .data; .sect .bss
.define .exg

	! #bytes in cx
.sect .text
.exg:
	pop     bx              ! return address
	mov     dx,cx
	mov     si,sp
	sub     sp,cx
	mov     di,sp
	rep movs
	mov     si,sp
	mov     di,sp
	add     di,dx
	add     di,dx
	mov     cx,dx
	sar     cx,1
	rep movs
	mov     sp,si
	jmp     bx
