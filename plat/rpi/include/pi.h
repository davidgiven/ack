/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#ifndef PI_H
#define PI_H

/* When running in kernel mode, this structure gets the incoming parameters.
 * In bare metal mode, it's gibberish. */

struct pi_kernel_parameters
{
	int r5;
	int r4;
	void* r3;
	void* r2;
	void* r1;
	void* r0;
};

extern struct pi_kernel_parameters* pi_kernel_parameters;

/* Initialise the mini UART (only do this if running on bare metal! */
extern void pi_init_uart(void);

/* Converts a pointer from a physical address to a user address. */
extern void* pi_phys_to_user(void* ptr);

/* Converts a pointer from a user address to a physical address. */
extern void* pi_user_to_phys(void* ptr);

#endif

