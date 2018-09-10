#
.sect .text; .sect .rom; .sect .data; .sect .bss

.sect .text
.define .c_ui_f
.c_ui_f:
	/* Input: r2
	 * Output: f0
	 * Only at and f30/f31 may be used.
	 */
	mtc1 r2, f0
	cvt.s.w f0, f0
	bgez r2, nonnegative
	nop

	li at, ha16[.fs_100000000]
	ldc1 f30, lo16[.fs_100000000] (at)
	add.d f0, f0, f30
nonnegative:
	jr ra
	nop

/* 4294967296 as a float. */
.sect .rom
.fs_100000000:
	.dataf4 4294967296.0
