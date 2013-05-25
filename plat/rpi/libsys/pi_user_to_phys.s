#
/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "libsysasm.h"

.sect .text

! Transforms a user address into a physical address.

.define _pi_user_to_phys
_pi_user_to_phys:
	ld r0, 0 (sp)
	add r0, gp
	b lr

