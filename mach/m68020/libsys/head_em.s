.define .lino,.filn
.define EXIT
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
	move.l	d0,(splimit)	! load splimit with the initial stack limit
	sub.l	#8,sp
	move.l	(8,sp),(sp)	! argc
	lea	(12,sp),a0
	move.l	a0,(4,sp)	! argv
	move.l	a0,a1
1:
	tst.l	(a0)+		! null args term?
	bne	1b
	cmp.l	(a1),a0		! end of 'argv[]' and in *argv[] strings?
	blt	2f		! skip if a0 is less than start of *argv[]
	sub.l	#4,a0		! else backup one to set env = 0 (NULL)
2:
	move.l	a0,(8,sp)	! env
!	move.l	a0, environ	! indir is 0 if no env; not 0 if env
!	jsr	(initfpu)	! call to dummy floating point init routine
	jsr	(__m_a_i_n)
	move.l	d0,(sp)		! no stack cleanup needed
EXIT:
	jsr	(__exit)

	.sect .data
begdata:
splimit:
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
