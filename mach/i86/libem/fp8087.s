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
	wait
	flds	2(bx)
	fadds	6(bx)
	fstps	6(bx)
	wait
	ret
.adf8:
	mov	bx,sp
	wait
	fldd	2(bx)
	faddd	10(bx)
	fstpd	10(bx)
	wait
	ret

.sbf4:
	mov	bx,sp
	wait
	flds	6(bx)
	fsubs	2(bx)
	fstps	6(bx)
	wait
	ret

.sbf8:
	mov	bx,sp
	wait
	fldd	10(bx)
	fsubd	2(bx)
	fstpd	10(bx)
	wait
	ret

.mlf4:
	mov	bx,sp
	wait
	flds	2(bx)
	fmuls	6(bx)
	fstps	6(bx)
	wait
	ret
.mlf8:
	mov	bx,sp
	wait
	fldd	2(bx)
	fmuld	10(bx)
	fstpd	10(bx)
	wait
	ret

.dvf4:
	mov	bx,sp
	wait
	flds	6(bx)
	fdivs	2(bx)
	fstps	6(bx)
	wait
	ret

.dvf8:
	mov	bx,sp
	wait
	fldd	10(bx)
	fdivd	2(bx)
	fstpd	10(bx)
	wait
	ret

.ngf4:
	mov	bx,sp
	wait
	flds	2(bx)
	fchs
	fstps	2(bx)
	wait
	ret

.ngf8:
	mov	bx,sp
	wait
	fldd	2(bx)
	fchs
	fstpd	2(bx)
	wait
	ret

.fif4:
	mov	bx,sp
	push	bx		! make room for FP status word
	wait
	flds	4(bx)
	fmuls	8(bx)		! multiply
	fld	st		! copy result
	ftst			! test sign; handle negative separately
	fstsw	-2(bx)
	wait
	mov	ax,-2(bx)
	sahf			! result of test in condition codes
	jb	1f
	frndint			! this one rounds (?)
	fcom	st(1)		! compare with original; if <=, then OK
	fstsw	-2(bx)
	wait
	mov	ax,-2(bx)
	sahf
	jbe	2f
	fisubs	(one)		! else subtract 1
	jmp	2f
1:				! here, negative case
	frndint			! this one rounds (?)
	fcom	st(1)		! compare with original; if >=, then OK
	fstsw	-2(bx)
	wait
	mov	ax,-2(bx)
	sahf
	jae	2f
	fiadds	(one)		! else add 1
2:
	fsub	st(1),st	! subtract integer part
	mov	bx,2(bx)
	fstps	(bx)
	fstps	4(bx)
	wait
	pop	bx
	ret

.fif8:
	mov	bx,sp
	push	bx		! make room for FP status word
	wait
	fldd	4(bx)
	fmuld	12(bx)		! multiply
	fld	st		! and copy result
	ftst			! test sign; handle negative separately
	fstsw	-2(bx)
	wait
	mov	ax,-2(bx)
	sahf			! result of test in condition codes
	jb	1f
	frndint			! this one rounds (?)
	fcom	st(1)		! compare with original; if <=, then OK
	fstsw	-2(bx)
	wait
	mov	ax,-2(bx)
	sahf
	jbe	2f
	fisubs	(one)		! else subtract 1
	jmp	2f
1:				! here, negative case
	frndint			! this one rounds (?)
	fcom	st(1)		! compare with original; if >=, then OK
	fstsw	-2(bx)
	wait
	mov	ax,-2(bx)
	sahf
	jae	2f
	fiadds	(one)		! else add 1
2:
	fsub	st(1),st	! subtract integer part
	mov	bx,2(bx)
	fstpd	(bx)
	fstpd	8(bx)
	wait
	pop	bx
	ret

.fef4:
				! this could be simpler, if only the
				! fxtract instruction was emulated properly
	mov	bx,sp
	mov	ax,6(bx)
	and	ax,077600
	je	1f		! zero exponent
	mov	cx,7
	shr	ax,cl
	sub	ax,126
	mov	cx,ax		! exponent in cx
	mov	ax,6(bx)
	and	ax,0100177
	or	ax,0037400	! load -1 exponent
	mov	dx,4(bx)
	mov	bx,2(bx)
	mov	4(bx),ax
	mov	2(bx),dx
	mov	(bx),cx
	ret
1:				! we get here on zero exp
	mov	ax,6(bx)
	and	ax,0177
	mov	6(bx),ax
	or	ax,4(bx)
	jne	1f		! zero result
	xor	ax,ax
	mov	bx,2(bx)
	mov	(bx),ax
	mov	2(bx),ax
	mov	4(bx),ax
	ret
1:				! otherwise unnormalized number
	mov	cx,6(bx)
	mov	dx,cx
	and	cx,0x8000
	mov	ax,-125
2:
	test	dx,0x80
	jne	1f
	dec	ax
	shl	4(bx),1
	rcl	dx,1
	or	dx,cx
	jmp	2b
1:
	mov	cx,4(bx)
	mov	bx,2(bx)
	mov	(bx),ax
	mov	2(bx),cx
	and	dx,0100177
	or	dx,0037400	! load -1 exponent
	mov	4(bx),dx
	ret

.fef8:
				! this could be simpler, if only the
				! fxtract instruction was emulated properly
	mov	bx,sp
	mov	ax,10(bx)
	and	ax,077760
	je	1f		! zero exponent
	mov	cx,4
	shr	ax,cl
	sub	ax,1022
	mov	cx,ax		! exponent in cx
	mov	ax,10(bx)
	and	ax,0100017
	or	ax,0037740	! load -1 exponent
	push	8(bx)
	push	6(bx)
	push	4(bx)
	mov	bx,2(bx)
	pop	2(bx)
	pop	4(bx)
	pop	6(bx)
	mov	8(bx),ax
	mov	(bx),cx
	ret
1:				! we get here on zero exp
	mov	ax,10(bx)
	and	ax,017
	mov	10(bx),ax
	or	ax,8(bx)
	or	ax,6(bx)
	or	ax,4(bx)
	jne	1f		! zero result
	xor	ax,ax
	mov	bx,2(bx)
	mov	(bx),ax
	mov	2(bx),ax
	mov	4(bx),ax
	mov	6(bx),ax
	mov	8(bx),ax
	ret
1:				! otherwise unnormalized number
	mov	cx,10(bx)
	mov	dx,cx
	and	cx,0x8000
	mov	ax,-1021
2:
	test	dx,0x10
	jne	1f
	dec	ax
	shl	4(bx),1
	rcl	6(bx),1
	rcl	8(bx),1
	rcl	dx,1
	or	dx,cx
	jmp	2b
1:
	and	dx,0100017
	or	dx,0037740	! load -1 exponent
	mov	cx,8(bx)
	push	6(bx)
	push	4(bx)
	mov	bx,2(bx)
	mov	(bx),ax
	mov	8(bx),dx
	mov	6(bx),cx
	pop	2(bx)
	pop	4(bx)
	ret

.cif4:
	mov	bx,sp
	cmp	2(bx),2
	jne	1f
	filds	4(bx)
	fstps	2(bx)
	wait
	ret
1:
	fildl	4(bx)
	fstps	4(bx)
	wait
	ret

.cif8:
	mov	bx,sp
	cmp	2(bx),2
	jne	1f
	filds	4(bx)
	fstpd	2(bx)
	wait
	ret
1:
	fildl	4(bx)
	fstpd	2(bx)
	wait
	ret

.cuf4:
	mov	bx,sp
	cmp	2(bx),2
	jne	1f
	mov	ax,4(bx)
	mov	2(bx),ax
	mov	4(bx),0
	fildl	2(bx)
	fstps	2(bx)
	wait
	ret
1:
	fildl	4(bx)
	cmp	6(bx),0
	jge	1f
2:
	fisubl	(bigmin)
	fisubl	(bigmin)
1:
	fstps	4(bx)
	wait
	ret

.cuf8:
	mov	bx,sp
	cmp	2(bx),2
	jne	1f
	mov	6(bx),0
1:
	fildl	4(bx)
	cmp	6(bx),0
	jge	1f
2:
	fisubl	(bigmin)
	fisubl	(bigmin)
1:
	fstpd	2(bx)
	wait
	ret

.cfi:
	mov	bx,sp
	push	bx
	fstcw	-2(bx)
	wait
	mov	dx,-2(bx)
	or	-2(bx),0xc00	! truncating mode
	wait
	fldcw	-2(bx)
	pop	ax
	cmp	4(bx),4
	jne	2f
				! loc 4 loc ? cfi
	flds	6(bx)
	fistpl	6(bx)
	wait
	cmp	2(bx),2
	jne	1f
	mov	ax,6(bx)
1:
	mov	4(bx),dx
	wait
	fldcw	4(bx)
	ret
2:
				! loc 8 loc ? cfi
	fldd	6(bx)
	fistpl	10(bx)
	wait
	cmp	2(bx),2
	jne	1b
	mov	ax,10(bx)
	jmp	1b

.cfu:
	mov	bx,sp
	push	bx
	fstcw	-2(bx)
	wait
	mov	dx,-2(bx)
	or	-2(bx),0xc00	! truncating mode
	wait
	fldcw	-2(bx)
	pop	ax
	cmp	4(bx),4
	jne	2f
				! loc 4 loc ? cfu
	flds	6(bx)
	fabs			! ???
	fistpl	6(bx)
	wait
	cmp	2(bx),2
	jne	1f
	mov	ax,6(bx)
1:
	mov	4(bx),dx
	wait
	fldcw	4(bx)
	ret
2:
				! loc 8 loc ? cfu
	fldd	6(bx)
	fabs			! ???
	fistpl	10(bx)
	wait
	cmp	2(bx),2
	jne	1b
	mov	ax,10(bx)
	jmp	1b

.cff4:
	mov	bx,sp
	wait
	fldd	2(bx)
	fstcw	2(bx)
	wait
	mov	dx,2(bx)
	and	2(bx),0xf3ff	! set to rounding mode
	wait
	fldcw	2(bx)
	fstps	6(bx)
	mov	2(bx),dx
	wait
	fldcw	2(bx)
	wait
	ret

.cff8:
	mov	bx,sp
	wait
	flds	2(bx)
	fstpd	2(bx)
	wait
	ret

.cmf4:
	mov	bx,sp
	push	bx		! room for 8087 status word
	xor	cx,cx
	flds	6(bx)
	flds	2(bx)
	fcompp			! compare and pop operands
	fstsw	-2(bx)
	wait
	mov	ax,-2(bx)
	sahf
	je	1f
	jb	2f
	dec	cx
	jmp	1f
2:
	inc	cx
1:
	mov	ax,cx
	pop	bx
	ret


.cmf8:
	mov	bx,sp
	push	bx		! room for 8087 status word
	xor	cx,cx
	fldd	10(bx)
	fldd	2(bx)
	fcompp			! compare and pop operands
	fstsw	-2(bx)
	wait
	mov	ax,-2(bx)
	sahf
	je	1f
	jb	2f
	dec	cx
	jmp	1f
2:
	inc	cx
1:
	mov	ax,cx
	pop	bx
	ret
