#
#include "asm.h"

/* Computes HL = BC * DE, corrupting DE. */
.define .mli3
.mli3:
    ld hl, 0
    ld a, 24            /* number of iterations */
loop:
    add hl, hl			/* top bit -> C */

	ex de, hl			/* rotate de left, add in C, top bit -> C */
	adc hl, hl
	ex de, hl

    jr nc, noadd
    add hl, bc
noadd:
    dec a
    jr nz, _loop
    ret
