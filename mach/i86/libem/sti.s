.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .sti

	! #bytes in cx
	! address in bx
	! save di/si. they might be register variables
.sti:
	mov	dx,di		! save di
	pop     ax              ! return address
	mov	di,bx
	mov	bx,ax
	sar     cx,1
	jnb     1f
	pop     ax
	stosb
	mov	di,dx
	jmp     bx
1:
	mov	ax,si
	mov     si,sp
	rep movs
	mov     sp,si
	mov	di,dx
	mov	si,ax
	jmp     bx
