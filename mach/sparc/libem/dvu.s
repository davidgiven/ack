.global dvu4
.global dvi4

dvu4:
! %o0' = %o0 / %o1
! %o1' = %o0 % %o1
	tst	%o1
	be	0f
	nop
	clr	%o5
	tst	%o0
	bge	3f	/* jump if msb clear */
	nop

	clr	%o2
	mov	%o1, %o3
	clr	%o4
1:	tst	%o1
	bl	5f	/* jump if msb set */
	nop
	sll	%o1, 1, %o1
	inc	%o4
	b	1b
	nop


dvi4:
! %o2 = %o0 / %o1
! %o3 = %o0 % %o1
	tst	%o1
	be	0f
	clr	%o5
	tst	%o0
	bge	1f
	nop
	neg	%o0
	xor	%o5, 1, %o5
1:
	tst	%o1
	bge	3f
	nop
	neg	%o1
	xor	%o5, 3, %o5

3:	/* div common */
	clr	%o2
	mov	%o1, %o3
	clr	%o4

4:
	cmp	%o1, %o0
	bgu	5f
	nop
	inc	%o4
	sll	%o1, 1, %o1
	b	4b
	nop

5:	/* div common for dvu4 && ( %o0 & 0x80000000) */
	sll	%o2, 1, %o2
	subcc	%o0, %o1, %o0
	bgeu	3f
	nop
	add	%o0, %o1, %o0
	b	4f
	nop
3:
	inc	%o2
4:
	srl	%o1, 1, %o1
	tst	%o4
	bz	5f
	nop
	dec	%o4
	b	5b
	nop

5:
	andcc	%o5, 1, %g0
	bz	6f
	nop
	tst	%o0
	bz	7f
	nop
	inc	%o2
	sub	%o3, %o0, %o0
7:
	neg	%o2
6:
	andcc	%o5, 2, %g0
	bz	8f
	nop
	neg	%o0
8:
	mov	%o0, %o1
	mov	%o2, %o0
	retl
	nop

0:					/* divide by zero */
	ta	2
