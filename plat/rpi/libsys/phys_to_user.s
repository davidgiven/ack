#
/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "libsysasm.h"

.sect .text

! Transforms a physical address into a user address.

.define _phys_to_user
_phys_to_user:
	ld r0, 0 (sp)
	sub r0, gp
	b lr

