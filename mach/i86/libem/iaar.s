.define .iaar

.iaar:
        ! $Header$
	pop     bx
	pop     dx
	cmp     dx,2
	jne     .unknown
	pop     si      ! descriptor address
	pop     ax      ! index
	pop     di      ! array base
	sub     ax,(si)
	mul     4(si)
	add     di,ax
	jmp     bx
