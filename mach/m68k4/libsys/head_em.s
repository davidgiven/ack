.define .lino,.filn
.define EXIT
.define	begtext,begdata,begbss
.define	EARRAY,ERANGE,ESET,EIDIVZ,EHEAP,EILLINS,ECASE,EBADGTO
.define	hol0,.reghp,.limhp,.trpim,.trppc
.sect .text
.sect .rom
.sect .data
.sect .bss



! EM runtime start-off for the Bleasdale 68000 system


LINO_AD	= 0
FILN_AD	= 4

EARRAY	= 0
ERANGE	= 1
ESET	= 2
EIDIVZ	= 6
EHEAP	= 17
EILLINS	= 18
ECASE	= 20
EBADGTO = 27

	.sect .text
begtext:
	! Bleasdale puts the argument and environment vectors
	! themselves on top of the stack, instead of POINTERS
	! to these vectors. We get things right here.
	move.l	4(sp),a0
	clr.l	-4(a0)
	move.l	sp,a0
	sub.l	#8,sp
	move.l	(a0),(sp)
	add.l	#4,a0
	move.l	a0,4(sp)
1:
	tst.l	(a0)+
	bne	1b
	move.l	4(sp),a1
	cmp.l	(a1),a0
	blt	2f
	sub.l	#4,a0
2:
	move.l	a0,8(sp)

	! Now the stack contains an argc (4 bytes), argv-pointer and
	! envp pointer.

	jsr	__m_a_i_n
	add	#012,sp
EXIT:
	move.l	d0,-(sp)
	jsr	__exit

	.sect .data
begdata:
hol0:
.lino:
	.data4	0	! lino
.filn:
	.data4	0	! filn
.reghp:
	.data4	endbss
.limhp:
	.data4	endbss
.trppc:
	.data4	0
.trpim:
	.data4	0


	.sect .bss
begbss: !initialization is not needed because ALL entries are in zero space!
