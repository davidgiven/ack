.define	CERASE,CKILL,CSTOP,CSTART
.define .lino,.filn
.define F_DUM,EXIT,WRITE,BRK
.define	begtext,begdata,begbss
.define	EARRAY,ERANGE,ESET,EIDIVZ,EHEAP,EILLINS,ECASE
.define	hol0,.reghp,.limhp,.trpim,.trppc
.define	LINO_AD,FILN_AD
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text




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

	.sect .text
begtext:
	jsr	__m_a_i_n
	add	#012,sp
EXIT:

	move.l	d0,-(sp)
	jsr __exit
	! As a temporary measure
	jsr .trp
	
WRITE:	jmp	__write

BRK:	jmp	__brk

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
	.data4	0	! was short


	.sect .bss
begbss:
