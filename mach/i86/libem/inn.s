.define .inn

        ! $Header$
	! #bytes in cx
	! bit # in ax
.inn:
	pop     bx              ! return address
	xor     dx,dx
	xor     si,si
	mov     di,8
	div     di
	mov     di,sp
	add     di,ax
	cmp     ax,cx
	xchg    ax,dx
	xchg    ax,si           ! ax:=si,si:=dx,does not change carry
	jae     1f
	movb    dl,bits(si)
	testb   (di),dl
	jz      1f
	inc     ax
1:
	add     sp,cx
	! ax is result
	jmp     bx

	.data
bits:
	.byte 1,2,4,8,16,32,64,128
