.define .adf4, .adf8, .sbf4, .sbf8, .mlf4, .mlf8, .dvf4, .dvf8
.define .ngf4, .ngf8, .fif4, .fif8, .fef4, .fef8
.define .cif4, .cif8, .cuf4, .cuf8, .cfi, .cfu, .cff4, .cff8
.define .cmf4, .cmf8
.sect .text; .sect .rom; .sect .data; .sect .bss

!	$Header$

!	Implement interface to floating point package for Intel 8087

	.sect .rom
one:
	.data2	1
two:
	.data2	2
bigmin:
	.data4 	-2147483648

	.sect .text
.adf4:
	mov	bx,sp
	flds	4(bx)
	fadds	8(bx)
	fstps	8(bx)
	wait
	ret
.adf8:
	mov	bx,sp
	fldd	4(bx)
	faddd	12(bx)
	fstpd	12(bx)
	wait
	ret

.sbf4:
	mov	bx,sp
	flds	8(bx)
	fsubs	4(bx)
	fstps	8(bx)
	wait
	ret

.sbf8:
	mov	bx,sp
	fldd	12(bx)
	fsubd	4(bx)
	fstpd	12(bx)
	wait
	ret

.mlf4:
	mov	bx,sp
	flds	4(bx)
	fmuls	8(bx)
	fstps	8(bx)
	wait
	ret
.mlf8:
	mov	bx,sp
	fldd	4(bx)
	fmuld	12(bx)
	fstpd	12(bx)
	wait
	ret

.dvf4:
	mov	bx,sp
	flds	8(bx)
	fdivs	4(bx)
	fstps	8(bx)
	wait
	ret

.dvf8:
	mov	bx,sp
	fldd	12(bx)
	fdivd	4(bx)
	fstpd	12(bx)
	wait
	ret

.ngf4:
	mov	bx,sp
	flds	4(bx)
	fchs
	fstps	4(bx)
	wait
	ret

.ngf8:
	mov	bx,sp
	fldd	4(bx)
	fchs
	fstpd	4(bx)
	wait
	ret

.fif4:
	mov	bx,sp
	flds	8(bx)
	fmuls	12(bx)		! multiply
	fld	st		! copy result
	ftst			! test sign; handle negative separately
	fstsw	ax
	wait
	sahf			! result of test in condition codes
	jb	1f
	frndint			! this one rounds (?)
	fcom	st(1)		! compare with original; if <=, then OK
	fstsw	ax
	wait
	sahf
	jbe	2f
	fisubs	(one)		! else subtract 1
	jmp	2f
1:				! here, negative case
	frndint			! this one rounds (?)
	fcom	st(1)		! compare with original; if >=, then OK
	fstsw	ax
	wait
	sahf
	jae	2f
	fiadds	(one)		! else add 1
2:
	fsub	st(1),st	! subtract integer part
	mov	bx,4(bx)
	fstps	(bx)
	fstps	4(bx)
	wait
	ret

.fif8:
	mov	bx,sp
	fldd	8(bx)
	fmuld	16(bx)		! multiply
	fld	st		! and copy result
	ftst			! test sign; handle negative separately
	fstsw	ax
	wait
	sahf			! result of test in condition codes
	jb	1f
	frndint			! this one rounds (?)
	fcom	st(1)		! compare with original; if <=, then OK
	fstsw	ax
	wait
	sahf
	jbe	2f
	fisubs	(one)		! else subtract 1
	jmp	2f
1:				! here, negative case
	frndint			! this one rounds (?)
	fcom	st(1)		! compare with original; if >=, then OK
	fstsw	ax
	wait
	sahf
	jae	2f
	fiadds	(one)		! else add 1
2:
	fsub	st(1),st	! subtract integer part
	mov	bx,4(bx)
	fstpd	(bx)
	fstpd	8(bx)
	wait
	ret

.fef4:
				! this could be simpler, if only the
				! fxtract instruction was emulated properly
	mov	bx,sp
	mov	ax,8(bx)
	and	ax,0x7f800000
	je	1f		! zero exponent
	shr	ax,23
	sub	ax,126
	mov	cx,ax		! exponent in cx
	mov	ax,8(bx)
	and	ax,0x807fffff
	or	ax,0x3f000000	! load -1 exponent
	mov	bx,4(bx)
	mov	4(bx),ax
	mov	(bx),cx
	ret
1:				! we get here on zero exp
	mov	ax,8(bx)
	and	ax,0x007fffff
	jne	1f		! zero result
	mov	bx,4(bx)
	mov	(bx),ax
	mov	4(bx),ax
	ret
1:				! otherwise unnormalized number
	mov	cx,8(bx)
	and	cx,0x807fffff
	mov	dx,cx
	and	cx,0x80000000
	mov	ax,-125
2:
	test	dx,0x800000
	jne	1f
	dec	ax
	shl	dx,1
	or	dx,cx
	jmp	2b
1:
	mov	bx,4(bx)
	mov	(bx),ax
	and	dx,0x807fffff
	or	dx,0x3f000000	! load -1 exponent
	mov	4(bx),dx
	ret

.fef8:
				! this could be simpler, if only the
				! fxtract instruction was emulated properly
	mov	bx,sp
	mov	ax,12(bx)
	and	ax,0x7ff00000
	je	1f		! zero exponent
	shr	ax,20
	sub	ax,1022
	mov	cx,ax		! exponent in cx
	mov	ax,12(bx)
	and	ax,0x800fffff
	or	ax,0x3fe00000	! load -1 exponent
	mov	dx,8(bx)
	mov	bx,4(bx)
	mov	4(bx),dx
	mov	8(bx),ax
	mov	(bx),cx
	ret
1:				! we get here on zero exp
	mov	ax,12(bx)
	and	ax,0xfffff
	or	ax,8(bx)
	jne	1f		! zero result
	mov	bx,4(bx)
	mov	(bx),ax
	mov	4(bx),ax
	mov	8(bx),ax
	ret
1:				! otherwise unnormalized number
	mov	cx,12(bx)
	and	cx,0x800fffff
	mov	dx,cx
	and	cx,0x80000000
	mov	ax,-1021
2:
	test	dx,0x100000
	jne	1f
	dec	ax
	shl	8(bx),1
	rcl	dx,1
	or	dx,cx
	jmp	2b
1:
	and	dx,0x800fffff
	or	dx,0x3fe00000	! load -1 exponent
	mov	cx,8(bx)
	mov	bx,4(bx)
	mov	(bx),ax
	mov	8(bx),dx
	mov	4(bx),cx
	ret

.cif4:
	mov	bx,sp
	fildl	8(bx)
	fstps	8(bx)
	wait
	ret

.cif8:
	mov	bx,sp
	fildl	8(bx)
	fstpd	4(bx)
	wait
	ret

.cuf4:
	mov	bx,sp
	fildl	8(bx)
	cmp	8(bx),0
	jge	1f
	fisubl	(bigmin)
	fisubl	(bigmin)
1:
	fstps	8(bx)
	wait
	ret

.cuf8:
	mov	bx,sp
	fildl	8(bx)
	cmp	8(bx),0
	jge	1f
	fisubl	(bigmin)
	fisubl	(bigmin)
1:
	fstpd	4(bx)
	wait
	ret

.cfi:
	mov	bx,sp
	fstcw	4(bx)
	wait
	mov	dx,4(bx)
	or	4(bx),0xc00	! truncating mode
	wait
	fldcw	4(bx)
	cmp	8(bx),4
	jne	2f
				! loc 4 loc ? cfi
	flds	12(bx)
	fistpl	12(bx)
1:
	mov	4(bx),dx
	wait
	fldcw	4(bx)
	ret
2:
				! loc 8 loc ? cfi
	fldd	12(bx)
	fistpl	16(bx)
	jmp	1b

.cfu:
	mov	bx,sp
	fstcw	4(bx)
	wait
	mov	dx,4(bx)
	and	4(bx),0xf3ff
	or	4(bx),0x400	! to -infinity
	wait
	fldcw	4(bx)
	cmp	8(bx),4
	jne	2f
				! loc 4 loc ? cfu
	flds	12(bx)
	fabs			! ???
	fiaddl	(bigmin)
	fistpl	12(bx)
	wait
	mov	ax,12(bx)
	sub	ax,(bigmin)
	mov	12(bx),ax
1:
	mov	4(bx),dx
	wait
	fldcw	4(bx)
	ret
2:
				! loc 8 loc ? cfu
	fldd	12(bx)
	fabs			! ???
	fiaddl	(bigmin)
	fistpl	16(bx)
	wait
	mov	ax,16(bx)
	sub	ax,(bigmin)
	mov	16(bx),ax
	jmp	1b

.cff4:
	mov	bx,sp
	fldd	4(bx)
	fstcw	4(bx)
	wait
	mov	dx,4(bx)
	and	4(bx),0xf3ff	! set to rounding mode
	wait
	fldcw	4(bx)
	fstps	8(bx)
	mov	4(bx),dx
	wait
	fldcw	4(bx)
	wait
	ret

.cff8:
	mov	bx,sp
	flds	4(bx)
	fstpd	4(bx)
	wait
	ret

.cmf4:
	mov	bx,sp
	xor	cx,cx
	flds	8(bx)
	flds	4(bx)
	fcompp			! compare and pop operands
	fstsw	ax
	wait
	sahf
	je	1f
	jb	2f
	dec	cx
	jmp	1f
2:
	inc	cx
1:
	mov	ax,cx
	ret


.cmf8:
	mov	bx,sp
	xor	cx,cx
	fldd	12(bx)
	fldd	4(bx)
	fcompp			! compare and pop operands
	fstsw	ax
	wait
	sahf
	je	1f
	jb	2f
	dec	cx
	jmp	1f
2:
	inc	cx
1:
	mov	ax,cx
	ret
