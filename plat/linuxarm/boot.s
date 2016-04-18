! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

begtext:
	! This code is placed at the beginning of the ELF executable and is the
	! first thing that runs.
	!
	! On entry, the stack looks like this:
	!
	! sp+..            NULL
	! sp+8+(4*argc)   env (X quads)
	! sp+4+(4*argc)   NULL
	! sp+4            argv (argc quads)
	! sp              argc
	!
	! On the ACK, this platform is using the pcc backend, so arguments go in
	! registers; we want:
	!
	! r0              argc
	! r1              argv
	! r2              env
	
	ldr r0, [sp]
	add r1, sp, #4
	add r2, sp, #8
	add r2, r2, r1, lsl #2

	ldr r3, __environp
	str r2, [r3]
	
	b __m_a_i_n

__environp:
	.data4 environ

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

