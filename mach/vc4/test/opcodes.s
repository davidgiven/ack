#
/*
 * VideoCore IV assembler test file
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

main:
	nop
	rti

	b r0
	b r31
	bl r0
	bl r31
	tbb r0
	tbb r15
	tbs r0
	tbs r15

	nop

	mov r0, r1
    cmn r0, r1
    add r0, r1
	bic r0, r1
	mul r0, r1
	eor r0, r1
	sub r0, r1
	and r0, r1
	mvn r0, r1
	ror r0, r1
	cmp r0, r1
	rsb r0, r1
	btst r0, r1
	or r0, r1
	extu r0, r1
	max r0, r1
	bset r0, r1
	min r0, r1
	bclr r0, r1
	adds2 r0, r1
	bchg r0, r1
	adds4 r0, r1
	adds8 r0, r1
	adds16 r0, r1
	exts r0, r1
	neg r0, r1
	lsr r0, r1
	clz r0, r1
	lsl r0, r1
	brev r0, r1
	asr r0, r1
	abs r0, r1

	nop

	mov.f r0, r1
    cmn.f r0, r1
    add.f r0, r1
	bic.f r0, r1
	mul.f r0, r1
	eor.f r0, r1
	sub.f r0, r1
	and.f r0, r1
	mvn.f r0, r1
	ror.f r0, r1
	cmp.f r0, r1
	rsb.f r0, r1
	btst.f r0, r1
	or.f r0, r1
	extu.f r0, r1
	max.f r0, r1
	bset.f r0, r1
	min.f r0, r1
	bclr.f r0, r1
	adds2.f r0, r1
	bchg.f r0, r1
	adds4.f r0, r1
	adds8.f r0, r1
	adds16.f r0, r1
	exts.f r0, r1
	neg.f r0, r1
	lsr.f r0, r1
	clz.f r0, r1
	lsl.f r0, r1
	brev.f r0, r1
	asr.f r0, r1
	abs.f r0, r1

	nop

	mov r0, r1, r2
    cmn r0, r1, r2
    add r0, r1, r2
	bic r0, r1, r2
	mul r0, r1, r2
	eor r0, r1, r2
	sub r0, r1, r2
	and r0, r1, r2
	mvn r0, r1, r2
	ror r0, r1, r2
	cmp r0, r1, r2
	rsb r0, r1, r2
	btst r0, r1, r2
	or r0, r1, r2
	extu r0, r1, r2
	max r0, r1, r2
	bset r0, r1, r2
	min r0, r1, r2
	bclr r0, r1, r2
	adds2 r0, r1, r2
	bchg r0, r1, r2
	adds4 r0, r1, r2
	adds8 r0, r1, r2
	adds16 r0, r1, r2
	exts r0, r1, r2
	neg r0, r1, r2
	lsr r0, r1, r2
	clz r0, r1, r2
	lsl r0, r1, r2
	brev r0, r1, r2
	asr r0, r1, r2
	abs r0, r1, r2

	nop

	mov r0, #0x1f
    cmn r0, #0x1f
    add r0, #0x1f
	bic r0, #0x1f
	mul r0, #0x1f
	eor r0, #0x1f
	sub r0, #0x1f
	and r0, #0x1f
	mvn r0, #0x1f
	ror r0, #0x1f
	cmp r0, #0x1f
	rsb r0, #0x1f
	btst r0, #0x1f
	or r0, #0x1f
	extu r0, #0x1f
	max r0, #0x1f
	bset r0, #0x1f
	min r0, #0x1f
	bclr r0, #0x1f
	adds2 r0, #0x1f
	bchg r0, #0x1f
	adds4 r0, #0x1f
	adds8 r0, #0x1f
	adds16 r0, #0x1f
	exts r0, #0x1f
	neg r0, #0x1f
	lsr r0, #0x1f
	clz r0, #0x1f
	lsl r0, #0x1f
	brev r0, #0x1f
	asr r0, #0x1f
	abs r0, #0x1f

	nop

	mov.f r0, #0x1f
    cmn.f r0, #0x1f
    add.f r0, #0x1f
	bic.f r0, #0x1f
	mul.f r0, #0x1f
	eor.f r0, #0x1f
	sub.f r0, #0x1f
	and.f r0, #0x1f
	mvn.f r0, #0x1f
	ror.f r0, #0x1f
	cmp.f r0, #0x1f
	rsb.f r0, #0x1f
	btst.f r0, #0x1f
	or.f r0, #0x1f
	extu.f r0, #0x1f
	max.f r0, #0x1f
	bset.f r0, #0x1f
	min.f r0, #0x1f
	bclr.f r0, #0x1f
	adds2.f r0, #0x1f
	bchg.f r0, #0x1f
	adds4.f r0, #0x1f
	adds8.f r0, #0x1f
	adds16.f r0, #0x1f
	exts.f r0, #0x1f
	neg.f r0, #0x1f
	lsr.f r0, #0x1f
	clz.f r0, #0x1f
	lsl.f r0, #0x1f
	brev.f r0, #0x1f
	asr.f r0, #0x1f
	abs.f r0, #0x1f

	add r0, #0x12345678
	add r0, r1, #0x12345678
	sub r0, #0x12345678

	nop

	fadd r0, r1, r2
	fsub r0, r1, r2
	fmul r0, r1, r2
	fdiv r0, r1, r2
	fcmp r0, r1, r2
	fabs r0, r1, r2
	frsb r0, r1, r2
	fmax r0, r1, r2
	frcp r0, r1, r2
	frsqrt r0, r1, r2
	fnmul r0, r1, r2
	fmin r0, r1, r2
	fld1 r0, r1, r2
	fld0 r0, r1, r2
	log2 r0, r1, r2
	exp2 r0, r1, r2
	divs r0, r1, r2
	divu r0, r1, r2
	divs r0, r1, #31
	divu r0, r1, #31
	adds256 r0, r1, r2

	nop

	fadd.f r0, r1, r2
	fsub.f r0, r1, r2
	fmul.f r0, r1, r2
	fdiv.f r0, r1, r2
	fcmp.f r0, r1, r2
	fabs.f r0, r1, r2
	frsb.f r0, r1, r2
	fmax.f r0, r1, r2
	frcp.f r0, r1, r2
	frsqrt.f r0, r1, r2
	fnmul.f r0, r1, r2
	fmin.f r0, r1, r2
	fld1.f r0, r1, r2
	fld0.f r0, r1, r2
	log2.f r0, r1, r2
	exp2.f r0, r1, r2
	divs.f r0, r1, r2
	divu.f r0, r1, r2
	divs.f r0, r1, #31
	divu.f r0, r1, #31
	adds256.f r0, r1, r2

label:
	b label
	b forward
	b label
	b main
	b.f label
	b.f forward
	b.f main
	bl label
	bl forward
	bl main
forward:

	push r0
	push r0, lr
	push r0-r5
	push r0-r5, lr
	push r6
	push r16
	push r24
	push lr

	pop r0
	pop r0, pc
	pop r0-r5
	pop r0-r5, pc
	pop r6
	pop r16
	pop r24
	pop pc
