/ $Header$
	.globl  LINO_AD,FILN_AD
	.globl  ERANGE,ESET,EHEAP,EILLINS,ECASE
	.globl  hol0,trppc~,trpim~,reghp~

rti     = 2
stst    = 170300 ^ tst

.float  = 1             / this should be parameterized somehow
.hardfp = 1             / only relevant if .float on

LINO_AD = 0.
FILN_AD = 4.

ERANGE  = 1.
ESET    = 2.
EFOVFL  = 4.
EFUNFL  = 5.
EFDIVZ  = 7.
EFUND   = 9.
ECONV   = 10.
EHEAP   = 17.
EILLINS = 18.
ECASE   = 20.

.if .float
/       .globl  fltused;        fltused:
.if 1 - .hardfp
/	sys     48.;4.;fptrap	/ if not commented it will appear as undefined
.endif
	sys     48.;8.;sig8
	ldfps   $7600
.endif
	mov     2(sp),r0
	clr     -2(r0)
	mov     sp,r0
	sub     $4,sp
	mov     4(sp),(sp)
	tst     (r0)+
	mov     r0,2(sp)
1:
	tst     (r0)+
	bne     1b
	cmp     r0,*2(sp)
	blo     1f
	tst     -(r0)
1:
	mov     r0,4(sp)
	jsr     pc,_m_a_i_n
/ next two lines for as long as tail needs printf
/	mov     r0,-(sp)
/	jsr     pc,*$_exit
	sys     1.

	.data
hol0:   0;0     / line no
	0;0     / file
trppc~: 0
trpim~: 0
reghp~: _end

	.text
sig8:
.if .float
	mov     r0,-(sp)
	stst    r0
	mov     1f(r0),-(sp)
	jsr     pc,trp~
	sys     48.;8.;sig8
	mov     (sp)+,r0
	rti

	.data
1:      EILLINS; EILLINS; EFDIVZ; ECONV; EFOVFL; EFUNFL; EFUND; EILLINS
	.text
.endif
