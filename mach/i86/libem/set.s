.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .set

	! #bytes in cx
	! bit # in ax
.set:
	pop     bx              ! return address
	xor     dx,dx
!ifdef create set
	mov     di,sp
	sub     di,cx
1:
	push    dx
	cmp     sp,di
	ja      1b
!endif
	mov     di,8
	div     di
	cmp     ax,cx
	jae     2f
	mov     di,sp
	add     di,ax
	mov     si,dx
	movb    dl,bits(si)
	orb     (di),dl
	jmp     bx
2:
.extern ESET
.extern .trp
	push    bx
	mov     ax,ESET
	push    ax
	jmp     .trp

	.sect .data
bits:
	.data1   1,2,4,8,16,32,64,128
