#
/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

#define gp r15
#define STACKSIZE 16*1024

! MAIN ENTRY POINT

begtext:
	! This empty space is required by the boot loader.

	b start
	.space 508 ! 512 minus space needed for branch instruction

	! Wipe the bss. This must happen absolutely first, because we need
	! to store the old system registers into it.

start:
	lea r6, begbss
	lea r7, endbss
	mov r8, #0
_1:
	stb r8, (r6)
	addcmpb.lt r6, #1, r7, _1

	! Set up system registers.

	lea gp, begtext
	st fp, .returnfp
	st sp, .returnsp
	st lr, .returnlr

	! Set up the new stack and save the kernel parameters to it.

	lea sp, .stack + STACKSIZE - 6*4

	sub r0, gp ! pointer
	st r0, 0 (sp)

	sub r1, gp ! pointer
	st r1, 4 (sp)

	sub r2, gp ! pointer
	st r2, 8 (sp)

	sub r3, gp ! pointer
	st r3, 12 (sp)

	! r4-r5 are not pointers and don't need adjusting
	st r4, 16 (sp)
	st r5, 20 (sp)

	sub r0, sp, gp
	st r0, _gpu_parameters

	! Push standard parameters onto the stack and go.
	
	mov r0, #0
	push r0                 ! envp
	push r0                 ! argv
	push r0                 ! argc

	! Call the language startup code.

	bl __m_a_i_n

	! Fall through to __exit if this returns.

.define __exit
__exit:
	mov r0, sr
	ld fp, .returnfp
	ld sp, .returnsp
	ld lr, .returnlr
	b lr

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.define begtext, begdata, begbss
.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .trppc, .ignmask, _errno
.comm .trppc, 4
.comm .ignmask, 4
.comm _errno, 4

! We store the stack pointer and return address on entry so that we can
! cleanly exit.

.comm .returnfp, 4
.comm .returnsp, 4
.comm .returnlr, 4

! User pointer to the GPU kernel parameter block.

.define _gpu_parameters
.comm _gpu_parameters, 4

! User stack.

.comm .stack, STACKSIZE

