/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#ifndef PI_H
#define PI_H

/* Initialise the mini UART (only do this if running on bare metal! */
extern void init_uart(void);

/* Converts a pointer from a physical address to a user address. */
extern void* phys_to_user(void* ptr);

/* Converts a pointer from a user address to a physical address. */
extern void* user_to_phys(void* ptr);

#endif

