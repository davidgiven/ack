#
/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "libsysasm.h"

.sect .text

#define PASSWD 0x5a000000
#define PLLC 5
#define OSC 1

#define A2W                 0x7e102000
#define A2W_PLLC_MULT       0x7e102020
#define A2W_PLLC_MULT2      0x7e102120
#define A2W_PLLC_MULT_FRACT 0x7e102220
#define A2W_PLLx_DIV        0x7e102620

#define CM                  0x7e101000
#define CM_VPU_CTL          0x7e101008
#define CM_VPU_DIV          0x7e10100c
#define CM_TIME_DIV         0x7e1010ec
#define CM_TIME_CTL         0x7e1010e8

#define hash #
#define copy(A) A
#define poke(A, V) \
	mov r0, copy(hash) V; mov r1, copy(hash) A; st r0, (r1)

! Changes the clock speed to 250MHz.

.define _pi_fast_mode
_pi_fast_mode:
	poke(A2W + 0x190, 0x5a000001)
	poke(A2W_PLLC_MULT_FRACT, PASSWD | 87380)
	poke(A2W_PLLC_MULT2, PASSWD | 52 | 0x1000)
	poke(A2W + 0x3c, 0x5a000100)
	poke(A2W + 0x38, 0x5a000000)
	poke(A2W + 0x34, 0x5a144000)
	poke(A2W + 0x30, 0x5a000000)
	poke(CM + 0x108, 0x5a000200)
	poke(CM + 0x108, 0x5a0002aa)
	poke(A2W + 0x2c, 0x5a000000)
	poke(A2W + 0x28, 0x5a400000)
	poke(A2W + 0x24, 0x5a000005)
	poke(A2W_PLLC_MULT, PASSWD | 52 | 0x555000)
	poke(A2W_PLLC_MULT2, PASSWD | 52 | 0x21000)
	poke(A2W + 0x2c, 0x5a000042)
	poke(A2W + 0x28, 0x5a500401)
	poke(A2W + 0x24, 0x5a004005)
	poke(A2W_PLLC_MULT, PASSWD | 52 | 0x555000)
	poke(A2W_PLLx_DIV, PASSWD | 2)
	poke(CM + 0x108, 0x5a0002ab)
	poke(CM + 0x108, 0x5a0002aa)
	poke(CM + 0x108, 0x5a0002a8)
	poke(CM_VPU_CTL, PASSWD | 0x200 | OSC | 0x40)
	poke(CM_VPU_DIV, PASSWD | [4 << 12])
	poke(CM_VPU_CTL, PASSWD | PLLC | 0x40)
	poke(CM_VPU_CTL, PASSWD | PLLC | 0x50)
	poke(CM_TIME_DIV, PASSWD | [19 << 12] | 819)
	poke(CM_TIME_CTL, PASSWD | OSC | 0x10)

	mov r0, #250000000
	st r0, _pi_clock_speed
	b lr

