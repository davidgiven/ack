.define .ilar

.ilar:
        ! $Header$
	pop     bx
	pop     dx
	cmp     dx,2
	jne     .unknown
	pop     di      ! descriptor address
	pop     ax      ! index
	pop     si      ! array base
	push    bx
	jmp    .lar2
