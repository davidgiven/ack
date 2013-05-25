#
/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "libsysasm.h"

.sect .text

! Because of the low system clock rate, this baud rate might be inaccurate
! So be careful with your serial/terminal, some adjustment may be necessary.
TARGET_BAUD_RATE = 115200

! System clock is running directly off the 19.2MHz crystal at initial reset
SYSTEM_CLOCK = 19200000

GPFSEL1 = 0x7e200004
GPSET0 = 0x7e20001C
GPCLR0 = 0x7e200028
GPPUD = 0x7e200094
GPPUDCLK0 = 0x7e200098

AUX_ENABLES = 0x7e215004
AUX_MU_IO_REG = 0x7e215040
AUX_MU_IER_REG = 0x7e215044
AUX_MU_IIR_REG = 0x7e215048
AUX_MU_LCR_REG = 0x7e21504C
AUX_MU_MCR_REG = 0x7e215050
AUX_MU_LSR_REG = 0x7e215054
AUX_MU_MSR_REG = 0x7e215058
AUX_MU_SCRATCH = 0x7e21505C
AUX_MU_CNTL_REG = 0x7e215060
AUX_MU_STAT_REG = 0x7e215064
AUX_MU_BAUD_REG = 0x7e215068

! Sets up the mini UART for use as a console.

.define _pi_init_uart
_pi_init_uart:
	! Configure TX and RX GPIO pins for Mini Uart function.
	mov	r1, #GPFSEL1
	ld	r0, (r1)
	and	r0, #~[7<<12]
	or	r0, #2<<12
	and	r0, #~[7<<15]
	or	r0, #2<<15
	st	r0, (r1)

	mov	r1, #GPPUD
	mov	r0, #0
	st	r0, (r1)

delay1:
	add	r0, #1
	cmp	r0, #150
	b.ne delay1

	mov	r1, #GPPUDCLK0
	mov	r0, #[1<<14]|[1<<15]
	st	r0, (r1)

	mov	r0, #0
delay2:
	add	r0, #1
	cmp	r0, #150
	b.ne delay2

	mov	r1, #GPPUDCLK0
	mov	r0, #0
	st	r0, (r1)

	! Set up serial port
	mov	r1, #AUX_ENABLES
	mov	r0, #1
	st	r0, (r1)

	mov	r1, #AUX_MU_IER_REG
	mov	r0, #0
	st	r0, (r1)

	mov	r1, #AUX_MU_CNTL_REG
	mov	r0, #0
	st	r0, (r1)

	mov	r1, #AUX_MU_LCR_REG
	mov	r0, #3
	st	r0, (r1)

	mov	r1, #AUX_MU_MCR_REG
	mov	r0, #0
	st	r0, (r1)

	mov	r1, #AUX_MU_IER_REG
	mov	r0, #0
	st	r0, (r1)

	mov	r1, #AUX_MU_IIR_REG
	mov	r0, #0xC6
	st	r0, (r1)

	mov	r1, #AUX_MU_BAUD_REG
	mov	r0, #[[SYSTEM_CLOCK/[TARGET_BAUD_RATE*8]]-1]
	st	r0, (r1)

	mov	r1, #AUX_MU_LCR_REG
	mov	r0, #3
	st	r0, (r1)

	mov	r1, #AUX_MU_CNTL_REG
	mov	r0, #3
	st	r0, (r1)

	b lr

! Send a single byte.

.define __sys_rawwrite
__sys_rawwrite:
	ld r0, (sp)
	mov r1, #AUX_MU_LSR_REG
	! loop until space available in Tx buffer
sendwait:
	ld	r2, (r1)
	and	r2, #0x20
	cmp	r2, #0x20
	b.ne sendwait

	mov	r1, #AUX_MU_IO_REG
	stb	r0, (r1)

	b lr

