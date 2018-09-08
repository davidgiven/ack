#
.sect .text
.sect .rom
.sect .data
.sect .bss

.data

.define .c_ud_i
.c_ud_i:
	/* Input: f0
	 * Output: r2
	 * Only at and f31 may be used.
	 */
	li at, hi(.fd_80000000)
	ldc1 f31, lo(.fd_800000000)(at)
	c.le.d 0, f31, f0
	bc1t toobig
	nop

	trunc.w.d f0, f0
	mfc1 r2, f0
	jr ra
	nop

toobig:
	sub.d f0, f0, f31
	trunc.w.d f0, f0
	mfc1 r2, f0
	addiu r2, r2, 0x8000
	jr ra
	nop
