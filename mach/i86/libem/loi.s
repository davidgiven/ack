.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .loi

	! #bytes in cx
	! address in bx
	! save si/di. they might be register variables
.loi:
	pop     ax
	mov	dx,si
	mov	si,bx
	mov	bx,ax
	mov     ax,cx
	sar     cx,1
	jnb     1f
	xorb    ah,ah
	lodsb
	mov	si,dx
	push    ax
	jmp     bx
1:
	sub     sp,ax
	mov	ax,di
	mov     di,sp
	rep movs
	mov	si,dx
	mov	di,ax
	jmp     bx
