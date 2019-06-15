/*
 * data.c - this is the initialization for the standard streams
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO && ACKCONF_WANT_EMULATED_FILE

struct FILE __stdin = {
	0, 0, _IOREAD, 0,
	(unsigned char*)NULL, (unsigned char*)NULL,
};

struct FILE __stdout = {
	0, 1, _IOWRITE, 0,
	(unsigned char*)NULL, (unsigned char*)NULL,
};

struct FILE __stderr = {
	0, 2, _IOWRITE | _IOLBF, 0,
	(unsigned char*)NULL, (unsigned char*)NULL,
};

FILE* __iotab[FOPEN_MAX] = {
	&__stdin,
	&__stdout,
	&__stderr,
	0
};

#endif
