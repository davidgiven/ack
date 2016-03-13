/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
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

#define gp r15

#endif
