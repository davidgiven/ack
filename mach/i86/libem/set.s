.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .set

	! #bytes in cx
	! bit # in ax
.set:
	pop     bx              ! return address
	xor     dx,dx
!ifdef create set
	sub	sp,cx
	push	bx
	push	di
	mov     bx,sp
	xor	di,di
	sar	cx,1
1:
	mov     4(bx)(di),dx
	add	di,2
	loop	1b
!endif
	mov     bx,8
	div     bx
	cmp     ax,di
	jae     2f
	mov	di,dx
	movb	dl,bits(di)
	mov     di,sp
	add     di,ax
	orb     4(di),dl
	pop	di
	ret
2:
.extern ESET
.extern .trp
	pop	di
	mov     ax,ESET
	jmp     .trp

	.sect .data
bits:
	.data1   1,2,4,8,16,32,64,128
