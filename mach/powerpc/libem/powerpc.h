#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

#define IFFALSE 4
#define IFTRUE 12
#define ALWAYS 20
#define DNZ 16

#define LT 0
#define GT 1
#define EQ 2
#define OV 3
	
