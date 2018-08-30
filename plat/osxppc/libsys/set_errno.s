.sect .text
.define .set_errno
.set_errno:
	lis r4, ha16[_errno]
	stw r3, lo16[_errno](r4)	! set errno
	li r3, -1			! return -1
	blr
