.global csb
ECASE=20

csb:
		! case descriptor in %o0
		! index in %o1
	ld	[%o0], %o2	! default jump address
	ld	[%o0+4], %o3	! # entries
	inc	8, %o0
	tst	%o3
	be	3f
	nop
	dec	%o3
	set	0xffff, %o4
	and	%o3, %o4, %o5
	sll	%o5, 3, %o3
1:
	ld	[%o0+%o3], %o4
	cmp	%o4, %o1
	be	2f
	nop
	deccc	8, %o3
	bge	1b
	nop
	b	3f
	nop
2:	
	inc	4, %o3
	ld	[%o0+%o3], %o2
3:
	tst	%o2
	be	4f
	nop
	jmp	%o2
	nop
4:
	set	ECASE, %o0
	set	trp, %g1
	jmp	%g1
	nop
