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

#define D(e) .define e; e

.sect .data

! Define various ACK error numbers. Note that these are *not* ANSI C
! errnos, and are used for different purposes.

D(ERANGE)         = 1
D(ESET)           = 2
D(EIDIVZ)         = 6
D(EHEAP)          = 17
D(EILLINS)        = 18
D(EODDZ)          = 19
D(ECASE)          = 20
D(EBADMON)        = 25

