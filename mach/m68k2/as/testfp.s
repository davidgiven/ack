.sect .text ; .sect .rom ; .sect .data ; .sect .bss ; .sect .text

! test for Motorola MC68881 floating point processor instructions
! compare result of disassembled output with source.

.define _m_a_i_n
_m_a_i_n:
blablabla:
! dyadic instructions
	fadd.l	12(a6),fp0
	fadd	fp0,fp1

	fcmp.l	12(a6),fp0
	fcmp	fp0,fp1

	fdiv.l	12(a6),fp0
	fdiv	fp0,fp1

	fmod.l	12(a6),fp0
	fmod	fp0,fp1

	fmul.l	12(a6),fp0
	fmul	fp0,fp1

	frem.l	12(a6),fp0
	frem	fp0,fp1

	fscale.l	12(a6),fp0
	fscale	fp0,fp1

	fsgldiv.l	12(a6),fp0
	fsgldiv	fp0,fp1

	fsglmul.l	12(a6),fp0
	fsglmul	fp0,fp1

	fsub.l	12(a6),fp0
	fsub	fp0,fp1

!monadic instructions
	fabs.p	12(a6),fp1
	fabs.x	fp1,fp2
	fabs	fp1

	facos.p	12(a6),fp1
	facos.x	fp1,fp2
	facos	fp1

	fasin.p	12(a6),fp1
	fasin.x	fp1,fp2
	fasin	fp1

	fatan.p	12(a6),fp1
	fatan.x	fp1,fp2
	fatan	fp1

	fatanh.p	12(a6),fp1
	fatanh.x	fp1,fp2
	fatanh	fp1

	fcos.p	12(a6),fp1
	fcos.x	fp1,fp2
	fcos	fp1

	fcosh.p	12(a6),fp1
	fcosh.x	fp1,fp2
	fcosh	fp1

	fetox.p	12(a6),fp1
	fetox.x	fp1,fp2
	fetox	fp1

	fetoxm1.p	12(a6),fp1
	fetoxm1.x	fp1,fp2
	fetoxm1	fp1

	fgetexp.p	12(a6),fp1
	fgetexp.x	fp1,fp2
	fgetexp	fp1

	fgetman.p	12(a6),fp1
	fgetman.x	fp1,fp2
	fgetman	fp1

	fint.p	12(a6),fp1
	fint.x	fp1,fp2
	fint	fp1

	fintrz.p	12(a6),fp1
	fintrz.x	fp1,fp2
	fintrz	fp1

	flogn.p	12(a6),fp1
	flogn.x	fp1,fp2
	flogn	fp1

	flognp1.p	12(a6),fp1
	flognp1.x	fp1,fp2
	flognp1	fp1

	flog10.p	12(a6),fp1
	flog10.x	fp1,fp2
	flog10	fp1

	flog2.p	12(a6),fp1
	flog2.x	fp1,fp2
	flog2	fp1

	fneg.p	12(a6),fp1
	fneg.x	fp1,fp2
	fneg	fp1

	fsin.p	12(a6),fp1
	fsin.x	fp1,fp2
	fsin	fp1

	fsinh.p	12(a6),fp1
	fsinh.x	fp1,fp2
	fsinh	fp1

	fsqrt.p	12(a6),fp1
	fsqrt.x	fp1,fp2
	fsqrt	fp1

	ftan.p	12(a6),fp1
	ftan.x	fp1,fp2
	ftan	fp1

	ftanh.p	12(a6),fp1
	ftanh.x	fp1,fp2
	ftanh	fp1

	ftentox.p	12(a6),fp1
	ftentox.x	fp1,fp2
	ftentox	fp1

	ftwotox.p	12(a6),fp1
	ftwotox.x	fp1,fp2
	ftwotox	fp1

!sincos
	fsincos.s	12(a6),fp0:fp1
	fsincos.x	fp2,fp0:fp1

!some moves
	fmove.l	12(a6),fp0
	fmove.l	fp1,12(a6)
	fmove.p	fp0,12(a6){d0}
	fmove.p	fp0,12(a6){#4}
	fmove.l	12(a6),fpcr
	!fmove.l	a0,fpcr			! error
	fmove.l	a0,fpiar
	fmovecr	#1,fp1
	!fmovem.x d1,(a0)+		! error
	fmovem.x d1,-(a0)
	fmovem.x fp0-fp3/fp5,-(a0)
	!fmovem.x -(a0),fp0-fp3/fp5	! error
	fmovem.x (a0)+,fp0-fp3/fp5

!some branches
	fbeq	blablabla
	fbne	blablabla
	fbgt	blablabla
	fbngt	blablabla
	fbge	blablabla
	fbnge	blablabla
	fblt	blablabla
	fbnlt	blablabla
	fble	blablabla
	fbnle	blablabla
	fbgl	blablabla
	fbngl	blablabla
	fbgle	blablabla
	fbngle	blablabla
	fbogt	blablabla
	fbule	blablabla
	fboge	blablabla
	fbult	blablabla
	fbolt	blablabla
	fbuge	blablabla
	fbole	blablabla
	fbugt	blablabla
	fbogl	blablabla
	fbueq	blablabla
	fbor	blablabla
	fbun	blablabla
	fbf	blablabla
	fbt	blablabla
	fbsf	blablabla
	fbst	blablabla
	fbseq	blablabla
	fbsne	blablabla

	fdbgl	d4,blablabla

!nop
	fnop

!fscc
	fsult	d4

!ftst
	ftst.p	12(a6)
	ftst	fp0

!save/restore
	fsave	12(a6)
	! fsave	(a5)+		! error
	frestore	12(a6)
	! frestore	-(a5)	! error

!trap
	ftrapeq
	ftrapeq.l #4
	ftrapeq.w #4
