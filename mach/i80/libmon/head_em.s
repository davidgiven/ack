.define hol0, argv, envp, begbss
.define .ignmask,.reghp,.trapproc,.fra
.define .retadr,.retadr1,.areg,.bcreg,.tmp1
.define block1, block2, block3
.define .stop
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.sect .text
	lxi h,0x1000		! stack will grow from 0x1000 downwards
	sphl
	lxi d,endbss
	lxi h,begbss		! clear bss
	mov a,e
	sub l
	mov e,a
	mov a,d
	sbb h
	mov d,a			! de now contains endbss - begbss
	mvi c,0

2:	mov m,c
	inx h
	dcx d
	mov a,e
	ora d
	jnz 2b

	lxi h,envp		! call main
	push h
	lxi h,argv
	push h
	lxi h,0
	push h
	call __m_a_i_n
.stop:	jmp 0xfb52
	
.sect .bss
begbss:
.trapproc:	.space	2
.ignmask:	.space	2
.sect .data
hol0:		.space	8
.reghp:		.data2	endbss
argv:		.data2	0
envp:		.data2	0
.retadr:	.space	2		! used to save return address
.retadr1:	.space	2		! reserve
.bcreg:		.space	2		! used to save localbase
.areg:		.space	1
.tmp1:		.space	2
.fra:		.space	8		! 8 bytes function return area
block1:		.space	4		! used by 32 bits divide and
block2:		.space	4		! multiply routines
block3:		.space	4

