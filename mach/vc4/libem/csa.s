#
/*
 * VideoCore IV support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "videocore.h"

.define	.csa
.sect .data
.csa:
	! on entry:
	!   r0 = un-fixed-up descriptor
	!   r1 = value
	add r0, gp

	ld r2, 4 (r0)            ! check lower bound

	b.lo r1, r2, default     ! jump to default if r1 < r2

	sub r1, r2               ! adjust value to be 0-based

	ld r2, 8 (r0)            ! check upper bound
	b.hi r1, r2, default     ! jump to default if r1 > r2

    add r1, #3
go:
    ld r1, (r0, r1)          ! load destination address
    add r1, gp
    b r1                     ! ...and go

default:
	mov r1, #0               ! index of default value
	b go
