#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* 2147483648 as a double; used as a pivot for double->unsigned and unsigned->double. */

.sect .rom
.define .fd_80000000
.fd_80000000:
	.data4 0x41e00000, 0

