.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .sim

        ! $Id$

.sim:
	movl    (sp)+,r0
	movl    (sp)+,.trpim    ! store ignore mask
	bbc     $5,.trpim,I3    ! floating underflow to be ignored?
	bicpsw  $0100
	jmp     (r0)
I3:
	bispsw  $0100           ! enable underflow trap
	jmp     (r0)
