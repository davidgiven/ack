.define .lino,.filn
.define EXIT,WRITE,BRK
.define	begtext,begdata,begbss
.define	EARRAY,ERANGE,ESET,EIDIVZ,EHEAP,EILLINS,ECASE,EBADGTO
.define	hol0,.reghp,.limhp,.trpim,.trppc
.sect .text
.sect .rom
.sect .data
.sect .bss



! EM runtime start-off for MINIX ST


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
	move.l	sp,a0
	move.l	(a0)+,d0
	move.l	d0,d1
	add.l   #1,d1
        asl.l   #2,d1           ! pointers are four bytes on 68000
        move.l  a0,a1
        add.l   d1,a1
        move.l  a1,-(sp)        ! push environ
        move.l  a0,-(sp)        ! push argv
        move.w  d0,-(sp)        ! push argc
	jsr	__m_a_i_n
	add	#010,sp
EXIT:
	move.w	d0,-(sp)
	jsr	__exit
L0:	bra	L0

BRK:	jmp	__brk

WRITE:	jmp	__write

	.sect .data
begdata:
hol0:
.lino:
	.data2	0,0	! lino
.filn:
	.data4	0	! filn
.reghp:
	.data4	endbss
.limhp:
	.data4	endbss
.trppc:
	.data4	0
.trpim:
	.data2	0


	.sect .bss
begbss: !initialization is not needed because ALL entries are in zero space!
