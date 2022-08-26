/*
 * © 2013 David Given
 * © 2022 TK Chia
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#ifndef LIBSYSASM_H
#define LIBSYSASM_H

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

! Size of transfer buffer, for 32-bit DPMI mode.

TRANSFER_BUFFER_SIZE = 32 * 1024

#endif
