/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef __OBJECT_INCLUDED__
#define __OBJECT_INCLUDED__

#include <stdio.h>

struct ar_hdr;
struct outhead;
struct outname;
struct outrelo;
struct outsect;
struct ranlib;
struct outname;

int wr_open(const char *f);
void wr_close(void);
void wr_ohead(const struct outhead *h);
void wr_sect(const struct outsect *s, unsigned int c);
void wr_outsect(int sectno);
void wr_emit(const char *b, long c);
void wr_putc(int c);
void wr_relo(const struct outrelo *r, unsigned int c);
void wr_name(const struct outname *n, unsigned int c);
void wr_string(const char *s, long c);


void wr_arhdr(FILE* fd, struct ar_hdr *a);
void wr_ranlib(FILE* fd, struct ranlib *r, long cnt);
void wr_int2(FILE* fd, int i);
void wr_long(FILE* fd, long l);
void wr_bytes(FILE* fd, const char *buf, long l);


int rd_open(const char *f);
int rd_fdopen(FILE* fd);
void rd_close(void);
void rd_ohead(struct outhead *h);
void rd_sect(struct outsect *s, unsigned int c);
void rd_outsect(int sectno);
void rd_emit(char *b, long c);
void rd_relo(struct outrelo *r, unsigned int c);
void rd_rew_relos(struct outhead *head);
void rd_name(struct outname *n, unsigned int c);
void rd_string(char *s, long c);

/** Reads an archive header and returns the filled
 *  up archive header structure. Returns 0 if failure
 *  otherwise returns 1.
 *
 *  @param[in] fd Opened file descriptor to read from.
 *  @param[out] a Archive header structure.
 *
 */
int rd_arhdr(FILE* fd, struct ar_hdr *a);
void rd_ranlib(FILE* fd, struct ranlib *r, long cnt);
int rd_int2(FILE* fd);
long rd_long(FILE* fd);
unsigned int rd_unsigned2(FILE* fd);
void rd_bytes(FILE* fd, char *buf, long l);
FILE* rd_fd(void);

#endif /* __OBJECT_INCLUDED__ */
