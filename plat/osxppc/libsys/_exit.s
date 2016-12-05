.sect .text
.define __exit
__exit:
	addi r0, r0, 1		! _exit
	lwz r3, 0(sp)		! status
	sc 0
