        # $Header$
.globl .sim

.sim:
	movl    (sp)+,r0
	movl    (sp)+,.trpim    # store ignore mask
	bbc     $5,.trpim,L3    # floating underflow to be ignored?
	bicpsw  $0100
	jmp     (r0)
L3:
	bispsw  $0100           # enable underflow trap
	jmp     (r0)
