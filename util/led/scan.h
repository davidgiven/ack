/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef __SCAN_H_INCLUDED__
#define __SCAN_H_INCLUDED__

#include "arch.h"

/*
 * Offsets of the pieces of the input module in core.
 */

#define IND_HEAD	((ind_t)0)
#define IND_SECT(x)	(IND_HEAD    +		      sizeof(struct outhead))
#define IND_NAME(x)	(IND_SECT(x) + (x).oh_nsect * sizeof(struct outsect))
#define IND_CHAR(x)	(IND_NAME(x) + (x).oh_nname * sizeof(struct outname))
#ifdef SYMDBUG
#define OFF_DBUG(x)	(OFF_CHAR(x) + (x).oh_nchar)
#endif /* SYMDBUG */

extern void seek(long pos);
extern void closefile(char* filename);
extern void get_archive_header(struct ar_hdr* archive_header);
extern void get_modul(void);
extern void skip_modul(struct outhead* head);
extern void startrelo(struct outhead* head);
extern struct outrelo* nextrelo(void);
extern char* getemit(struct outhead* head, struct outsect* sects, int sectindex);
extern char* getblk(long totalsz, long* pblksz, int sectindex);
extern void endemit(char* emit);
/*
 * Open the file with name `filename' (if necessary) and examine the first
 * few bytes to see if it's a plain file or an archive.
 * In case of a plain file, the file pointer is repositioned after the
 * examination. Otherwise it is at the beginning of the table of contents.
 */
int getfile(char* filename);

#endif /* __SCAN_H_INCLUDED__ */
