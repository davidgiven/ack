.define .cms

        ! $Header$
	! #bytes in cx
.cms:
	pop     bx              ! return address
	mov     dx,sp
	mov     si,dx
	add     dx,cx
	mov     di,dx
	add     dx,cx
	sar     cx,1
	repe cmps
	je      1f
	inc     cx
1:
	mov     sp,dx
	jmp     bx
