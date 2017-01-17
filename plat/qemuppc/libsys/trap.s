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

.define .trap_ecase
.trap_ecase:
	b .trp

.define .trap_earray
.trap_earray:
	b .trp

.define .trap_erange
.trap_erange:
	b .trap

.define .trp
.define .trap
.trp:
.trap:
	b .trp					! spin forever
