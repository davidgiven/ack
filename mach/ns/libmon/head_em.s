.define hol0,argv,.reghp,.limhp,envp,begbss
.define WRITE
.define .ignmask, .trpreg
.define .stop, BRK
.define ERANGE,EILLINS,ECASE
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

topmem	= 0xFFFFFF

ERANGE 	= 1
EILLINS	= 18
ECASE	= 20

	.sect .text

	movd begbss, r7
	movd endbss, r6
1:
	movqd 0, 0(r7)
	addqd 4, r7
	cmpd r6,r7
	bgt 1b

	movd envp,tos
	movd argv,tos
	movd 1,tos
	jsr @__m_a_i_n
.stop:
	movb 0x7f,tos
	jsr @.putchar
	bpt

BRK:
	movd	0,r0
	ret	0

WRITE:	jump	@__write

.sect .bss
begbss:
.sect .data
hol0:
	.data4 0
	.data4 0
argv:
	.data4 3f
envp:
	.data4 0
3:
	.asciz 'PROGRAM'
.reghp:
	.data4 endbss
.limhp:
	.data4 endbss
.ignmask:
	.data2 0
.trpreg:
	.data4 0
