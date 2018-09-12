#
.sect .text; .sect .rom; .sect .data; .sect .bss

.sect .text
.define .c_ui_d
.c_ui_d:
	/* Input: r2
	 * Output: f0
	 * Only at and f30/f31 may be used.
	 */
	mtc1 r2, f0
	cvt.d.w f0, f0
	bgez r2, nonnegative
	nop

	lui at, ha16[.fd_100000000]
	ldc1 f30, lo16[.fd_100000000] (at)
	add.d f0, f0, f30
nonnegative:
	jr ra
	nop

.sect .rom
.fd_100000000:
	.dataf8 4294967296.0
