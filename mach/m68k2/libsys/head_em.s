.define	CERASE,CKILL,CSTOP,CSTART
.define .lino,.filn

.define F_DUM,EXIT

.define	begtext,begdata,begbss
.define	EARRAY,ERANGE,ESET,EIDIVZ,EHEAP,EILLINS,ECASE,EBADGTO
.define	hol0,.reghp,.limhp,.trpim,.trppc
.define	LINO_AD,FILN_AD

! EM runtime start-off for the Bleasdale 68000 system


CERASE	= 010
CKILL	= 030
CSTART	= 021
CSTOP	= 023
F_DUM	= 0


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

.base 0x20000
	.text
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

	add.l	#2,sp !convert argc from 4-byte to 2-byte
	pea	endbss
	jsr	_brk
	add.l	#4,sp
	jsr	_m_a_i_n
	add	#010,sp
EXIT:
	jsr	__exit

	.data
begdata:
hol0:
.lino:
	.short	0,0	! lino
.filn:
	.long	0	! filn
.reghp:
	.long	endbss
.limhp:
	.long	endbss
.trppc:
	.long	0
.trpim:
	.short	0


	.bss
begbss:
