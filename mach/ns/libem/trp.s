.define .trp
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.trp:	
	save [r0, r1]
	movd 12(sp), r0		!error number
	movd r0,tos
	cmpd r0, 16
	bge 1f
	tbitd r0, @.ignmask
	bfs 3f 			!do not trap
1:				!do trap
	movd @.trpreg, r1
	cmpqd 0, r1
	beq 2f
	movqd 0, @.trpreg
	jsr r1
3:
	adjspd $-4
	restore [r0, r1]
	ret 4
2:	
	jump @.stop		!no trapprocedure supplied
