.sect .text; .sect .rom; .sect .data;
.sect .data
.define _m_a_i_n
blablabla:
	.space 100

.sect .text

_m_a_i_n:
	! no operands
	f2xm1
	fabs
	fchs
	fclex
	fcompp
	fdecstp
	fdisi
	feni
	fincstp
	finit
	fld1
	fldl2e
	fldl2t
	fldlg2
	fldln2
	fldpi
	fldz
	fnop
	fpatan
	fprem
	fptan
	frndint
	fscale
	fsqrt
	ftst
	fxam
	fxtract
	fyl2x
	fyl2pi

	! memory operand
	fiadds	(blablabla)
	fiaddl	(blablabla)
	fadds	(blablabla)
	faddd	(blablabla)
	fbld	(blablabla)
	fbstp	(blablabla)
	ficoms	(blablabla)
	ficoml	(blablabla)
	fcoms	(blablabla)
	fcomd	(blablabla)
	ficomps	(blablabla)
	ficompl	(blablabla)
	fcomps	(blablabla)
	fcompd	(blablabla)
	fidivs	(blablabla)
	fidivl	(blablabla)
	fdivs	(blablabla)
	fdivd	(blablabla)
	fidivrs	(blablabla)
	fidivrl	(blablabla)
	fdivrs	(blablabla)
	fdivrd	(blablabla)
	filds	(blablabla)
	fildl	(blablabla)
	flds	(blablabla)
	fldd	(blablabla)
	fldx	(blablabla)
	fldcw	(blablabla)
	fldenv	(blablabla)
	fimuls	(blablabla)
	fimull	(blablabla)
	fmuls	(blablabla)
	fmuld	(blablabla)
	frstor	(blablabla)
	fsave	(blablabla)
	fists	(blablabla)
	fistl	(blablabla)
	fsts	(blablabla)
	fstd	(blablabla)
	fistps	(blablabla)
	fistpl	(blablabla)
	fstps	(blablabla)
	fstpd	(blablabla)
	fstpx	(blablabla)
	fstcw	(blablabla)
	fstenv	(blablabla)
	fstsw	(blablabla)
	fisubs	(blablabla)
	fisubl	(blablabla)
	fsubs	(blablabla)
	fsubd	(blablabla)
	fisubrs	(blablabla)
	fisubrl	(blablabla)
	fsubrs	(blablabla)
	fsubrd	(blablabla)

	! again, memory operand
	fiadds	8(bp)
	fiaddl	8(bp)
	fadds	8(bp)
	faddd	8(bp)
	fbld	8(bp)
	fbstp	8(bp)
	ficoms	8(bp)
	ficoml	8(bp)
	fcoms	8(bp)
	fcomd	8(bp)
	ficomps	8(bp)
	ficompl	8(bp)
	fcomps	8(bp)
	fcompd	8(bp)
	fidivs	8(bp)
	fidivl	8(bp)
	fdivs	8(bp)
	fdivd	8(bp)
	fidivrs	8(bp)
	fidivrl	8(bp)
	fdivrs	8(bp)
	fdivrd	8(bp)
	filds	8(bp)
	fildl	8(bp)
	flds	8(bp)
	fldd	8(bp)
	fldx	8(bp)
	fldcw	8(bp)
	fldenv	8(bp)
	fimuls	8(bp)
	fimull	8(bp)
	fmuls	8(bp)
	fmuld	8(bp)
	frstor	8(bp)
	fsave	8(bp)
	fists	8(bp)
	fistl	8(bp)
	fsts	8(bp)
	fstd	8(bp)
	fistps	8(bp)
	fistpl	8(bp)
	fstps	8(bp)
	fstpd	8(bp)
	fstpx	8(bp)
	fstcw	8(bp)
	fstenv	8(bp)
	fstsw	8(bp)
	fisubs	8(bp)
	fisubl	8(bp)
	fsubs	8(bp)
	fsubd	8(bp)
	fisubrs	8(bp)
	fisubrl	8(bp)
	fsubrs	8(bp)
	fsubrd	8(bp)

	! one FP stack argument
	fld	st
	fst	st
	fstp	st
	fxch	st
	fcom	st
	fcomp	st
	ffree	st

	fld	st(4)
	fst	st(4)
	fstp	st(4)
	fxch	st(4)
	fcom	st(4)
	fcomp	st(4)
	ffree	st(4)

	! two FP stack arguments
	fadd	st(4),st
	faddp	st(4),st
	fdiv	st(4),st
	fdivp	st(4),st
	fdivr	st(4),st
	fdivrp	st(4),st
	fmul	st(4),st
	fmulp	st(4),st
	fsub	st(4),st
	fsubp	st(4),st
	fsubr	st(4),st
	fsubrp	st(4),st

	fadd	st,st(4)
	faddp	st,st(4)
	fmul	st,st(4)
	fmulp	st,st(4)
