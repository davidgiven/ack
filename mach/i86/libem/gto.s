.define .gto

.gto:
        ! $Header$
	mov     bp,4(bx)
	mov     sp,2(bx)
	jmp     (bx)
