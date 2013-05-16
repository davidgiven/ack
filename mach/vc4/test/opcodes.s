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

	mov r0, #31
    cmn r0, #31
    add r0, #31
	bic r0, #31
	mul r0, #31
	eor r0, #31
	sub r0, #31
	and r0, #31
	mvn r0, #31
	ror r0, #31
	cmp r0, #31
	rsb r0, #31
	btst r0, #31
	or r0, #31
	extu r0, #31
	max r0, #31

