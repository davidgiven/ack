/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	ocm_chan.h - channel definitions */
#include <stdio.h>
#include "ocm_parco.h"

typedef union channel {
	struct {		/* Interprocess channel */
		char _type;	/* Channel type, see note */
		char synch;	/* State in channel synchronization */
		long val;	/* Transmitted value */
	} c;
	struct {		/* File channel */
		char _type;	/* Dummy field, see note */
		char index;	/* Index in the file array */
		char flgs;	/* Status flags: in use & readahead */
		char preread;	/* Possible preread character */
	} f;
} chan;
#define type		c._type	/* Channel type */
/* Note: The channel type should not be part of each structure in chan. But
 * the C alignment rules would make chan about 50% bigger if we had done it
 * the right way. Note that the order of fields in a struct cannot be a problem
 * as long as struct c is the largest within the union.
 */

#define C_T_CHAN	0	/* Type of a interprocess channel */
#define C_T_FILE	1	/* Type of a file channel */

#define C_S_FREE	0	/* IP channel is free */
#define C_S_ANY		1	/* IP channel contains data */
#define C_S_ACK		2	/* IP channel data is removed */

#define C_F_EOF		(-1L)	/* File channel returns EOF */
#define C_F_TEXT	(-2L)	/* File channel becomes line oriented */
#define C_F_RAW		(-3L)	/* File channel becomes character oriented */

#define C_F_INUSE	0x01	/* File channel is connected to a UNIX file */
#define C_F_READAHEAD	0x02	/* File channel has a preread character */

extern chan file[20];	/* Array of file channels */
extern FILE *unix_file[20];	/* Pointers to buffered UNIX files */

void c_init();

void chan_in(), cbyte_in(), c_wa_in(), c_ba_in();
void chan_out(), c_wa_out(), c_ba_out();

int chan_any();
