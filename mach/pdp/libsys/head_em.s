#
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  LINO_AD,FILN_AD
.define  ERANGE,ESET,EHEAP,EILLINS,ECASE
.define  hol0,trppc~,trpim~,reghp~,.limhp
! $Header$

#define float 1
#define hardfp 1

LINO_AD = 0
FILN_AD = 4

ERANGE  = 1
ESET    = 2
EFOVFL  = 4
EFUNFL  = 5
EFDIVZ  = 7
EFUND   = 011
ECONV   = 012
EHEAP   = 021
EILLINS = 022
ECASE   = 024

#ifdef float
#ifndef hardfp
!	sys     060;.data2 4,fptrap	/ if not commented it will appear as undefined
#endif
	sys     060;.data2 010,sig8
	ldfps   $07600
#endif
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
	jsr     pc,__m_a_i_n
! next two lines for as long as tail needs printf
!	mov     r0,-(sp)
!	jsr     pc,*$_exit
	sys     1

	.sect .data
hol0:   .data2 0,0     ! line no
	.data2 0,0     ! file
trppc~: .data2 0
trpim~: .data2 0
reghp~: .data2 _end
.limhp: .data2 _end

	.sect .text
sig8:
#ifdef float
	mov     r0,-(sp)
	stst    r0
	mov     1f(r0),-(sp)
	jsr     pc,trp~
	sys     060;.data2 010,sig8
	mov     (sp)+,r0
	rti

	.sect .data
1:      .data2 EILLINS, EILLINS, EFDIVZ, ECONV, EFOVFL, EFUNFL, EFUND, EILLINS
	.sect .text
#endif
