        # $Header$
.globl  .lxa

.lxa:
	jsb	.lxl	# Find local base
	jsb	.lpb	# Convert to argument base
	rsb
