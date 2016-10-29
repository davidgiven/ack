#
! $Source: /cvsroot/tack/Ack/plat/linux386/libsys/_syscall.s,v $
! $State: Exp $
! $Revision: 1.1 $

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

#define IFFALSE 4
#define IFTRUE 12
#define ALWAYS 20

#define LT 0
#define GT 1
#define EQ 2
#define OV 3

EARRAY	=  0
ERANGE	=  1
ESET	=  2
EIOVFL	=  3
EFOVFL	=  4
EFUNFL	=  5
EIDIVZ	=  6
EFDIVZ	=  7
EIUND	=  8
EFUND	=  9
ECONV	= 10
ESTACK  = 16
EHEAP	= 17
EILLINS = 18
EODDZ	= 19
ECASE	= 20
EMEMFLT	= 21
EBADPTR = 22
EBADPC  = 23
EBADLAE = 24
EBADMON = 25
EBADLIN = 26
EBADGTO = 27
EUNIMPL = 63		! unimplemented em-instruction called

! EM trap handling.

.define .trap_ecase
.trap_ecase:
	addi r3, r0, ECASE
	b .trap
	
.define .trap_earray
.trap_earray:
	addi r3, r0, EARRAY
	b .trap
	
.define .trap
.trap:
	cmpi cr0, 0, r3, 15      ! traps >15 can't be ignored
	bc IFTRUE, LT, 1f
	
	addi r4, r0, 1
	rlwnm r4, r4, r3, 0, 31  ! calculate trap bit
	li32 r5, .ignmask
	lwz r5, 0(r5)            ! load ignore mask
	and. r4, r4, r5          ! compare
	bclr IFFALSE, EQ, 0      ! return if non-zero
	
1:
	li32 r4, .trppc
	lwz r5, 0(r4)            ! load user trap routine
	or. r5, r5, r5           ! test
	bc IFTRUE, EQ, fatal     ! if no user trap routine, bail out
	
	addi r0, r0, 0
	stw r0, 0(r4)            ! reset trap routine
	
	mfspr r0, lr
	stwu r0, -4(sp)          ! save old lr
	
	stwu r3, -4(sp)
	mtspr ctr, r5
	bcctrl ALWAYS, 0, 0      ! call trap routine
	
	lwz r0, 4(sp)            ! load old lr again
	addi sp, sp, 8           ! retract over stack usage
	bclr ALWAYS, 0, 0        ! return 

fatal:
	addi r3, r0, 1
	li32 r4, message
	addi r5, r0, 6
	addi r0, r0, 4           ! write()
	sc 0
	
	addi r0, r0, 1           ! exit()
	sc 0

.sect .rom
message:
	.ascii "TRAP!\n"
