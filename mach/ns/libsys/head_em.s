.define hol0,argv,.reghp,envp,begbss
.define .ignmask, .trpreg
.define .stop
.define ERANGE,EILLINS,ECASE

base	= 0x10000
topmem	= 0xFFFFFF

ERANGE 	= 1
EILLINS	= 18
ECASE	= 20

	.base base
	.text

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
	jsr @_m_a_i_n
.stop:
	movb 0x7f,tos
	jsr @.putchar
	bpt

.bss
begbss:
.data
hol0:
	.long 0
	.long 0
argv:
	.long 3f
envp:
	.long 0
3:
	.asciz 'PROGRAM'
.reghp:
	.long endbss
.ignmask:
	.short 0
.trpreg:
	.long 0
