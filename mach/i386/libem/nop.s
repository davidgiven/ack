.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .nop
.extern printd, printc, hol0

SIO_S           = 0xDA
SIO_D           = 0xD8
RXRDY           = 0x02

.nop:
	mov     eax,(hol0)
	call    printd
!       movb    al,' '
!       call    printc
!       mov     eax,esp
!       call    printd
!1:
!       inb     SIO_S
!       andb    al,RXRDY
!       jz      1b
!       inb     SIO_D
!       call    printc
	movb    al,'\n'
	jmp     printc
