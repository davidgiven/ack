.define .exg

        ! $Header$
	! #bytes in cx
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
