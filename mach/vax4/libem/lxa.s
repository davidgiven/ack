.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .lxa

        ! $Id$

.lxa:
	jsb	.lxl	! Find local base
	jsb	.lpb	! Convert to argument base
	rsb
