.define .nop

SIO_S           = 0xDA
SIO_D           = 0xD8
RXRDY           = 0x02

.nop:
        ! $Header$
	mov     ax,(hol0)
	call    printd
!       movb    al,' '
!       call    printc
!       mov     ax,sp
!       call    printd
!1:
!       inb     SIO_S
!       andb    al,RXRDY
!       jz      1b
!       inb     SIO_D
!       call    printc
	movb    al,'\n'
	jmp     printc
