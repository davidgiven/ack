.global aar
.global lar
.global sar

aar:	! %o0 = descr	%o1 = index	%o2 = array_ptr

	save	%sp, -64, %sp
	ld	[%i0], %o0	! lower bound
	subcc	%i1, %o0, %o1	! 0..N-1
	bl	1f		! <0
	nop
	ld	[%i0+4], %o0	! n
	cmp	%o1, %o0
	bgt	1f		! > N
	nop
	call	mli4		! descr *= n.size
	ld	[%i0+8], %o0	! el.size
	add	%i2, %o0, %i0	! &base_addr[n]
	ret
	restore			! delay slot
1:	restore	
	set	EARRAY, %o0
	call	trp
	nop
	retl
	nop

lar:	! %o0 = descr	%o1 = index	%o2 = array_ptr
	
	mov	%o7, %o4
	call	aar
	mov	%o0, %o3
	mov	%o4, %o7
	
	! %o0 = address of el, %o1 = sizeof(el), %o2 = word, %o3 = descr
	ld	[%o3+8], %o1	! el.size
	cmp	%o1, 1
	bne	2f	
	nop
	ldub	[%o0], %o2	! el.size == 1
	dec	4, %l0
	retl
	st	%o2, [%l0]	! delay slot
2:	cmp	%o1,2
	bne	3f
	nop
	lduh	[%o0], %o2	! el.size == 2
	dec	4, %l0
	retl
	st	%o2, [%l0]	! delay slot
3:	cmp	%o1, 4
	bne	4f
	nop
	ld	[%o0], %o2
	dec	4, %l0
	retl
	st	%o2, [%l0]	! delay slot
4:	sub	%l0, %o1, %l0
5:	deccc	4, %o1
	ld	[%o0+%o1], %o2
	bnz	5b
	st	%o2, [%l0+%o1]
	retl
	nop

sar:	! %o0 = descr	%o1 = index	%o2 = array_ptr
	
	mov	%o7, %o4
	call	aar
	mov	%o0, %o3
	mov	%o4, %o7
	
	! %o0 = address of el, %o1 = sizeof(el), %o2 = word, %o3 = descr

	ld	[%o3+8], %o1	! el.size
	cmp	%o1, 1
	bne	2f	
	nop
	ld	[%l0], %o2
	stb	%o2, [%o0]
	retl
	inc	4, %l0		! delay slot
2:	cmp	%o1,2
	bne	3f
	nop
	ld	[%l0], %o2
	sth	%o2, [%o0]
	retl
	inc	4, %l0		! delay slot
3:	cmp	%o1, 4
	bne	4f
	nop
	ld	[%l0], %o2
	st	%o2, [%o0]
	retl
	inc	4, %l0		! delay slot

4:	mov	%o1, %o4
5:	deccc	4, %o1
	ld	[%l0+%o1], %o2
	bnz	5b
	st	%o2, [%o0+%o1]	! delay slot
	retl
	add	%l0, %o4, %l0	! delay slot
