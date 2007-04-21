#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

begtext:
	! This code is placed at the beginning of the ELF executable and is the
	! first thing that runs.

	jmp __m_a_i_n
	
	! This provides an emergency exit routine used by EM.
	
.define EXIT
.extern EXIT
EXIT:
	push 1
	jmp __exit
	
! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .trppc, .ignmask
.comm .trppc, 4
.comm .ignmask, 4
