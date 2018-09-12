#
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.define .c_uf_i
.c_uf_i:
	/* Input: f0
	 * Output: r2
	 * Only at and f30/f31 may be used.
	 */

	lui at, ha16[.ff_80000000]
	lwc1 f30, lo16[.ff_80000000] (at)
	c.le.s 0, f30, f0
	bc1t toobig
	nop

	trunc.w.s f0, f0
	mfc1 r2, f0
	jr ra
	nop

toobig:
	sub.s f0, f0, f30
	trunc.w.s f0, f0
	mfc1 r2, f0
	lui at, 0x8000			! load 0x80000000
	addu r2, r2, at
	jr ra
	nop

.sect .rom
.ff_80000000:
	.dataf4 2147483648.0

