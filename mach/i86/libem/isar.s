.define .isar

.isar:
        ! $Header$
	pop     bx
	pop     dx
	cmp     dx,2
	jne     .unknown
	pop     si      ! descriptor address
	pop     ax      ! index
	pop     di      ! array base
	push    bx
	jmp    .sar2
