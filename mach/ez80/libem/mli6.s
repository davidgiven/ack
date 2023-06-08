#
#include "asm.h"

/* Computes HLHL = BCBC * DEDE, corrupting DEDE. */
.define .mli6
.mli6:
    ld hl, 0
	exx
	ld hl, 0
	exx
    ld a, 24            /* number of iterations */
loop:
    add hl, hl			/* top bit -> C */
	exx
	adc hl, hl
	exx

	ex de, hl			/* rotate de left, add in C, top bit -> C */
	adc hl, hl
	ex de, hl
	exx
	ex de, hl
	adc hl, hl
	ex de, hl
	exx

    jr nc, noadd
    add hl, bc
	exx
	adc hl, bc
	exx
noadd:
    dec a
    jr nz, _loop
    ret
