#
/*
 * VideoCore IV support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "videocore.h"

.define	.csb
.sect .data
.csb:
	! on entry:
	!   r0 = un-fixed-up descriptor
	!   r1 = value
	add r0, gp               ! r0 = fixed up descriptor

	adds8 r2, r0, #1         ! r2 = moving pointer
	ld r3, 4 (r0)            ! r3 = count
	adds8 r3, r0, r3         ! r3 = end ptr

loop:
	ld r4, (r2)++
	b.eq r4, r1, matched      ! r2 points at matching addr
	addcmpb.le r2, #4, r3, loop
notmatched:
	mov r2, r0               ! r2 points at default jump
matched:
	ld r2, (r2)              ! load destination address
    add r2, gp               ! fix up r2
    b r2                     ! ...and go

