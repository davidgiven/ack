.define .trp

	.text

.trp:	
	save [r0, r1]
	movd 12(sp), r0		!error number
	cmpd r0, 16
	bge 1f
	sbitd r0, r1
	andw @.ignmask, r1
	beq 1f
	br 3f 			!do not trap
1:				!do trap
	movd @.trpreg, r1
	cmpqd 0, r1
	beq 2f
	movqd 0, @.trpreg
	jsr r1
3:
	restore [r0, r1]
	ret 4
2:	
	jump @.stop		!no trapprocedure supplied
