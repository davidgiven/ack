.define .lino,.filn
.define EXIT, WRITE, BRK
.define	begtext,begdata,begbss
.define	EARRAY,ERANGE,ESET,EIDIVZ,EHEAP,EILLINS,ECASE,EBADGTO
.define	hol0,.reghp,.limhp,.trpim,.trppc
.sect .text
.sect .rom
.sect .data
.sect .bss



! runtime startof for 68020 machine


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
	move.l	(sp),d2
	lea	(4,sp),a0
	move.l	d2,d1
	add.l	#1,d1
	asl.l	#2,d1
	move.l	a0,a1
	add.l	d1,a1
	pea	(a1)
	pea	(a0)
	move.l	d2,-(sp)
	move.l	#0,a6
	jsr	(__m_a_i_n)
	move.l	d0,(sp)		! no stack cleanup needed
EXIT:
	jsr	(__exit)

WRITE:
	jmp	(__write)

BRK:
	jmp	(__brk)

	.sect .data
begdata:
	.data4 0		! may be at virtual address 0 with no problem
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
	.data4	0	! USED TO BE 2 BYTES; IS THIS RIGHT?


	.sect .bss
begbss: !initialization is not needed because ALL entries are in zero space!
